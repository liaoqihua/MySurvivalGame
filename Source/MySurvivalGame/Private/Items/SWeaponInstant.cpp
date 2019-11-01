// Fill out your copyright notice in the Description page of Project Settings.

#include "SWeaponInstant.h"
#include "SCharacter.h"
#include "DrawDebugHelpers.h"
#include "UnrealNetwork.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SWeapon.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "SImpactEffect.h"
#include "Engine/World.h"

ASWeaponInstant::ASWeaponInstant(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer), WeaponRange(50000.0f), AllowedViewDotHitDir(0.8f), ClientSideHitLeeway(0.5), HitDamage(10.0f),
	MinimumProjectileSpawnDistance(800.0f), TracerRoundInterval(3)
{
	TimeBetweenShots = 0.1f;
	TrailTargetParam = TEXT("ShockBeamEnd");
}

void ASWeaponInstant::FireWeapon()
{
	const FVector AimDir = GetAdjustedAim();
	const FVector StartPos = GetCameraDamageStartLocation(AimDir);
	const FVector EndPos = StartPos + (AimDir * WeaponRange);

	const FHitResult HitResult = WeaponTrace(StartPos, EndPos);
	ProcessInstantHit(HitResult, StartPos, AimDir);
}

void ASWeaponInstant::ProcessInstantHit(const FHitResult & Impact, const FVector & Origin, const FVector & ShootDir)
{
	if (OwnerPawn && OwnerPawn->IsLocallyControlled() && GetNetMode() == NM_Client) {
		//服务器拥有的物体
		if (Impact.GetActor() && Impact.GetActor()->GetRemoteRole() == ROLE_Authority) {
			ServerNotifyHit(Impact, ShootDir);
		}
		else if (Impact.GetActor() == nullptr) {
			if (Impact.bBlockingHit) {
				ServerNotifyHit(Impact, ShootDir);
			}
			else {
				ServerNotifyMiss(ShootDir);
			}
		}
	}

	ProcessInstantHitConfirmed(Impact, Origin, ShootDir);
}

void ASWeaponInstant::ProcessInstantHitConfirmed(const FHitResult & Impact, const FVector & Origin, const FVector & ShootDir)
{
	//UKismetSystemLibrary::PrintString(GetWorld(), TEXT("ProcessInstantHitConfirmed"), true, true, FLinearColor::Green, 5.0f);
	if (ShouldDealDamage(Impact.GetActor())) {
		DealDamage(Impact, ShootDir);
	}

	if (Role == ROLE_Authority) {
		HitOriginNotify.OriginPoint = Origin;
		HitOriginNotify.EnsureReplication();
	}

	if (GetNetMode() != NM_DedicatedServer) {
		SimulateInstantHit(Origin);
	}
}

void ASWeaponInstant::DealDamage(const FHitResult & Impact, const FVector & ShootDir)
{
	if (DamageTypeClass) {
		FPointDamageEvent PointDamage;
		PointDamage.DamageTypeClass = DamageTypeClass;
		PointDamage.HitInfo = Impact;
		PointDamage.ShotDirection = ShootDir;
		PointDamage.Damage = HitDamage;

		Impact.GetActor()->TakeDamage(PointDamage.Damage, PointDamage, OwnerPawn->GetController(), this);
	}
}

bool ASWeaponInstant::ShouldDealDamage(AActor * TestActor) const
{
	if (TestActor) {
		if (GetNetMode() != NM_Client || TestActor->Role == ROLE_Authority || TestActor->GetTearOff()) {
			//UKismetSystemLibrary::PrintString(GetWorld(), TEXT("ShouldDealDamage"), true, true, FLinearColor::Red, 5.0f);
			return true;
		}
	}
	return false;
}

void ASWeaponInstant::OnRep_HitOriginNotify()
{
	SimulateInstantHit(HitOriginNotify.OriginPoint);
}

void ASWeaponInstant::SimulateInstantHit(const FVector & Origin)
{
	//DrawDebugPoint(GetWorld(), Origin, 20, FColor::Blue, false, 5.0f);

	const FVector StartTrace = Origin;
	const FVector AimDir = GetAdjustedAim();

	const FVector EndTrace = StartTrace + (AimDir * WeaponRange);
	const FHitResult Impact = WeaponTrace(StartTrace, EndTrace);
	if (Impact.bBlockingHit) {
		SpawnImpactEffects(Impact);
		SpawnTrailEffects(EndTrace);
	}
	else {
		SpawnTrailEffects(EndTrace);
	}
}

void ASWeaponInstant::SpawnImpactEffects(const FHitResult & Impact)
{
	ASImpactEffect *EffectActor = GetWorld()->SpawnActorDeferred<ASImpactEffect>(ImpactTemplate, FTransform(Impact.ImpactPoint.Rotation(), Impact.ImpactPoint), this, OwnerPawn, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

	if (EffectActor) {
		EffectActor->SurfaceHit = Impact;
		UGameplayStatics::FinishSpawningActor(EffectActor, FTransform(Impact.ImpactPoint.Rotation(), Impact.ImpactPoint));
	}
}

void ASWeaponInstant::SpawnTrailEffects(const FVector & EndPoint)
{
	BulletsShotCount++;

	const FVector Origin = GetMuzzleLocation();
	FVector ShootDir = EndPoint - Origin;

	if (ShootDir.Size() < MinimumProjectileSpawnDistance) return;

	if ((BulletsShotCount % TracerRoundInterval) == 0) {
		if (TracerFX) {
			ShootDir.Normalize();
			UGameplayStatics::SpawnEmitterAtLocation(this, TracerFX, Origin, ShootDir.Rotation());
		}
	}
	else {
		if (OwnerPawn && OwnerPawn->IsLocallyControlled()) return;

		if (TrailFX) {
			UParticleSystemComponent *TrailPSC = UGameplayStatics::SpawnEmitterAtLocation(this, TrailFX, Origin);
			if (TrailPSC) {
				TrailPSC->SetVectorParameter(TrailTargetParam, EndPoint);
			}
		}
	}
}

void ASWeaponInstant::ServerNotifyMiss_Implementation(FVector_NetQuantizeNormal ShootDir)
{
	const FVector Origin = GetMuzzleLocation();

	HitOriginNotify.OriginPoint = Origin;
	HitOriginNotify.EnsureReplication();

	const FVector EndTrace = Origin + (ShootDir * WeaponRange);

	if (GetNetMode() != NM_DedicatedServer) {
		SpawnTrailEffects(EndTrace);
	}
}

bool ASWeaponInstant::ServerNotifyMiss_Validate(FVector_NetQuantizeNormal ShootDir)
{
	return true;
}

void ASWeaponInstant::ServerNotifyHit_Implementation(const FHitResult Impact, FVector_NetQuantizeNormal ShootDir)
{
	if (Instigator && (Impact.GetActor() || Impact.bBlockingHit)) {
		const FVector Origin = GetMuzzleLocation();
		const FVector ViewDir = (Impact.Location - Origin).GetSafeNormal();

		const float ViewDotHitDir = FVector::DotProduct(Instigator->GetViewRotation().Vector(), ViewDir);

		if (ViewDotHitDir > AllowedViewDotHitDir) {
			// TODO: Check for weapon state
			//如果没有碰撞到Actor，但是被阻挡了
			if (Impact.GetActor() == nullptr)
			{
				if (Impact.bBlockingHit)
				{
					ProcessInstantHitConfirmed(Impact, Origin, ShootDir);
				}
			}
			//由于静态物体是不移动的，通常这种命中没有太多的游戏意义
			// Assume it told the truth about static things because we don't move and the hit
			// usually doesn't have significant gameplay implications
			else if (Impact.GetActor()->IsRootComponentStatic() || Impact.GetActor()->IsRootComponentStationary())
			{
				ProcessInstantHitConfirmed(Impact, Origin, ShootDir);
			}
			else //碰撞到动态物体
			{
				//获得碰撞物体的边界盒
				const FBox HitBox = Impact.GetActor()->GetComponentsBoundingBox();

				// CenterLoc = min + (max - min) / 2 = min + max/2 - min/2 = max/2 + min/2
				const FVector BoxCenter = (HitBox.Min + HitBox.Max) * 0.5;

				//求最大与最小点之间的中点位置
				// Loc = (max - min) /2
				FVector BoxExtent = 0.5 * (HitBox.Max - HitBox.Min);
				DrawDebugBox(GetWorld(), BoxCenter, BoxExtent, FColor::Green, true, 5.0f);

				BoxExtent *= ClientSideHitLeeway;

				//限定盒子的大小最小为 20 * 20 * 20
				BoxExtent.X = FMath::Max(20.0f, BoxExtent.X);
				BoxExtent.Y = FMath::Max(20.0f, BoxExtent.Y);
				BoxExtent.Z = FMath::Max(20.0f, BoxExtent.Z);
				DrawDebugBox(GetWorld(), BoxCenter, BoxExtent, FColor::Red, true, 5.0f);

				DrawDebugPoint(GetWorld(), Impact.Location, 20, FColor::Blue, false, 5.0f);

				//如果碰撞点与边界盒中心距离小于，盒子边界。如果大于就算没打到
				// If we are within client tolerance
				if (FMath::Abs(Impact.Location.Z - BoxCenter.Z) < BoxExtent.Z &&
					FMath::Abs(Impact.Location.X - BoxCenter.X) < BoxExtent.X &&
					FMath::Abs(Impact.Location.Y - BoxCenter.Y) < BoxExtent.Y)
				{
					ProcessInstantHitConfirmed(Impact, Origin, ShootDir);
				}
			}

		}
	}
}

bool ASWeaponInstant::ServerNotifyHit_Validate(const FHitResult Impact, FVector_NetQuantizeNormal ShootDir)
{
	return true;
}

void ASWeaponInstant::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASWeaponInstant, HitOriginNotify, COND_SkipOwner);

}