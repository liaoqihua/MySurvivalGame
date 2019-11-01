// Fill out your copyright notice in the Description page of Project Settings.

#include "SWeapon.h"
#include <Components/SkeletalMeshComponent.h>
#include <UnrealNetwork.h>
#include "SCharacter.h"
#include <Components/AudioComponent.h>
#include <Animation/AnimMontage.h>
#include <Sound/SoundCue.h>
#include <TimerManager.h>
#include <Kismet/GameplayStatics.h>
#include "SPlayerController.h"
#include "MySurvivalGame.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
ASWeapon::ASWeapon(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer), bWantsToFire(false), TimeBetweenShots(0.5f), bRefiring(false), BurstCounter(0),
	MuzzleAttachPoint("MuzzleFlashSocket"), bPlayingFireAnim(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, "Mesh");

	SetReplicates(true);
	bNetUseOwnerRelevancy = true;

	CurrentState = EWeaponState::Idle;
	StorageSlot = EInventorySlot::Primary;

}

// Called when the game starts or when spawned
void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

USkeletalMeshComponent * ASWeapon::GetWeaponMesh() const
{
	return Mesh;
}

ASCharacter * ASWeapon::GetPawnOwner() const
{
	return OwnerPawn;
}

EWeaponState ASWeapon::GetCurrentState() const
{
	return CurrentState;
}

void ASWeapon::OnEnterInventory(ASCharacter *NewOwner)
{
	if (NewOwner) {
		SetOwningPawn(NewOwner);
		AttachMeshToPawn(StorageSlot);
	}
}

void ASWeapon::SetOwningPawn(ASCharacter *NewOwner)
{
	if (OwnerPawn != NewOwner) {
		OwnerPawn = NewOwner;
		Instigator = NewOwner;
		SetOwner(NewOwner);
	}
}

void ASWeapon::AttachMeshToPawn(EInventorySlot Slot /*= EInventorySlot::Hands*/)
{
	if (OwnerPawn) {
		DetachMeshFromPawn();

		USkeletalMeshComponent *PawnMesh = OwnerPawn->GetMesh();
		FName AttachPoint = OwnerPawn->GetInventoryAttachPoint(Slot);
		Mesh->SetHiddenInGame(false);
		Mesh->AttachToComponent(PawnMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, AttachPoint);
	}
}

void ASWeapon::DetachMeshFromPawn()
{
	Mesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	Mesh->SetHiddenInGame(true);
}

void ASWeapon::OnRep_OwnerPawn()
{
	if (Role < ROLE_Authority) {
		if (OwnerPawn) {
			OnEnterInventory(OwnerPawn);
		}
		else {
			DetachMeshFromPawn();
		}
	}
}

void ASWeapon::OnEquip()
{
	AttachMeshToPawn();

	bPendingEquip = true;
	DetermineWeaponState();

	float Duration = PlayWeaponAnimation(EquipAnim);
	if (Duration <= 0.0f) {
		Duration = 0.5f;
	}

	EquipStartedTime = GetWorld()->TimeSeconds;
	EquipDuration = Duration;
	GetWorldTimerManager().SetTimer(EquipFinishedTimerHandle, this, &ASWeapon::OnEquipFinished, Duration, false);
	if (OwnerPawn && OwnerPawn->IsLocallyControlled()) {
		PlayWeaponSound(EquipSound);
	}
}

void ASWeapon::OnUnEquip()
{
	AttachMeshToPawn(StorageSlot);

	bIsEquiped = false;
	if (bPendingEquip) {
		StopWeaponAnimation(EquipAnim);
		bPendingEquip = false;
		GetWorldTimerManager().ClearTimer(EquipFinishedTimerHandle);
	}
	DetermineWeaponState();
}

void ASWeapon::OnEquipFinished()
{
	AttachMeshToPawn();

	bIsEquiped = true;
	bPendingEquip = false;
	DetermineWeaponState();
}

UAudioComponent * ASWeapon::PlayWeaponSound(USoundCue * SoundToPlay)
{
	UAudioComponent *AudioComp = nullptr;
	if (SoundToPlay && OwnerPawn) {
		AudioComp = UGameplayStatics::SpawnSoundAttached(SoundToPlay, OwnerPawn->GetRootComponent());
	}
	return AudioComp;
}

void ASWeapon::SetWeaponState(EWeaponState NewState)
{
	//可能需要做一些开火 开始和结束的特效之类的
	const EWeaponState PrevState = CurrentState;

	//停止发射
	if (PrevState == Firing && NewState != Firing) {
		//UKismetSystemLibrary::PrintString(GetWorld(), FString(TEXT("OnWeaponFinish")), true, true, FLinearColor::MakeRandomColor(), 15.0f);
		OnBurstFinished();
	}

	CurrentState = NewState;

	//开始发射
	if (PrevState != Firing && NewState == Firing) {
		//UKismetSystemLibrary::PrintString(GetWorld(), FString(TEXT("OnWeaponStart")), true, true, FLinearColor::MakeRandomColor(), 15.0f);
		OnBurstStarted();
	}

	//CurrentState = NewState;
}

void ASWeapon::DetermineWeaponState()
{
	EWeaponState NewState = Idle;
	if (bIsEquiped) {
		if (bWantsToFire && CanFire()) NewState = Firing;
	}
	else if (bPendingEquip) {
		NewState = Equipping;
	}

	SetWeaponState(NewState);
}

float ASWeapon::PlayWeaponAnimation(UAnimMontage * Animation, float InPlayRate, FName StartSectionName)
{
	float Duration = 0.f;

	if (OwnerPawn && Animation) {
		Duration = OwnerPawn->PlayAnimMontage(Animation, InPlayRate, StartSectionName);
	}

	return Duration;
}

void ASWeapon::StopWeaponAnimation(UAnimMontage *Animation)
{
	if (OwnerPawn && Animation) OwnerPawn->StopAnimMontage(Animation);
}

void ASWeapon::OnLeaveInventory()
{
	if (Role == ROLE_Authority) {
		SetOwningPawn(nullptr);
	}

	if (IsAttachedToPawn()) {
		OnUnEquip();
	}
	DetachMeshFromPawn();
}

bool ASWeapon::IsEquiped() const
{
	return bIsEquiped;
}

bool ASWeapon::IsAttachedToPawn() const
{
	return bIsEquiped || bPendingEquip;
}

void ASWeapon::StartFire()
{
	if (Role < ROLE_Authority) {
		ServerStartFire();
	}

	if (!bWantsToFire) {
		bWantsToFire = true;
		DetermineWeaponState();
	}
}

void ASWeapon::StopFire()
{
	if (Role < ROLE_Authority) {
		ServerStopFire();
	}

	if (bWantsToFire) {
		bWantsToFire = false;
		DetermineWeaponState();
	}
}

void ASWeapon::ServerStartFire_Implementation()
{
	StartFire();
}

bool ASWeapon::ServerStartFire_Validate()
{
	return true;
}

void ASWeapon::ServerStopFire_Implementation()
{
	StopFire();
}

bool ASWeapon::ServerStopFire_Validate()
{
	return true;
}

bool ASWeapon::CanFire() const
{
	bool bPawnCanFire = OwnerPawn && OwnerPawn->CanFire();
	bool bStatOk = CurrentState == EWeaponState::Idle || CurrentState == EWeaponState::Firing;

	return bPawnCanFire && bStatOk;
}

void ASWeapon::OnBurstStarted()
{
	const float NowTime = GetWorld()->GetTimeSeconds();
	if (LastFireTime > 0.0f && TimeBetweenShots > 0.0f && LastFireTime + TimeBetweenShots > NowTime) {
		GetWorldTimerManager().SetTimer(FiringTimerHandle, this, &ASWeapon::HandleFiring, LastFireTime + TimeBetweenShots - NowTime, false);
	}
	else HandleFiring();

}

void ASWeapon::OnBurstFinished()
{
	BurstCounter = 0;

	if (GetNetMode() != NM_DedicatedServer) StopSimulateWeaponFire();

	GetWorldTimerManager().ClearTimer(FiringTimerHandle);
	bRefiring = false;
}

void ASWeapon::HandleFiring()
{
	UKismetSystemLibrary::PrintString(GetWorld(), FString(TEXT("HandleFiring")), true, true, FLinearColor::Red, 15.0f);
	if (CanFire())
	{
		if (GetNetMode() != NM_DedicatedServer)
		{
			SimulateWeaponFire();
		}
		if (OwnerPawn && OwnerPawn->IsLocallyControlled())
		{
			FireWeapon();
			// TODO: Consume Ammo
			// Update firing FX on remote clients if this is called on server
			BurstCounter++;
		}
	}

	if (OwnerPawn && OwnerPawn->IsLocallyControlled()) {
		if (Role < ROLE_Authority) ServerHandleFiring();

		bRefiring = (CurrentState == Firing && TimeBetweenShots > 0.0f);
		if (bRefiring) {
			GetWorldTimerManager().SetTimer(FiringTimerHandle, this, &ASWeapon::HandleFiring, TimeBetweenShots, false);
		}
	}

	LastFireTime = GetWorld()->GetTimeSeconds();
}

void ASWeapon::SimulateWeaponFire()
{
	UKismetSystemLibrary::PrintString(GetWorld(), FString(TEXT("SimulateWeaponFire")), true, true, FLinearColor::MakeRandomColor(), 15.0f);

	if (MuzzleFX) {
		MuzzlePSC = UGameplayStatics::SpawnEmitterAttached(MuzzleFX, Mesh, MuzzleAttachPoint);
	}

	if (!bPlayingFireAnim) {
		if (FireAnim)
			PlayWeaponAnimation(FireAnim);
		bPlayingFireAnim = true;
	}

	if (FireSound) PlayWeaponSound(FireSound);
}

void ASWeapon::StopSimulateWeaponFire()
{
	UKismetSystemLibrary::PrintString(GetWorld(), FString(TEXT("StopSimulateWeaponFires")), true, true, FLinearColor::MakeRandomColor(), 15.0f);

	if (bPlayingFireAnim) {
		if (FireAnim)
			StopWeaponAnimation(FireAnim);
		bPlayingFireAnim = false;
	}
}

void ASWeapon::OnRep_BurstCounter()
{
	if (BurstCounter > 0) {
		SimulateWeaponFire();
	}
	else {
		StopSimulateWeaponFire();
	}
}

FVector ASWeapon::GetAdjustedAim() const
{
	ASPlayerController *PC = Instigator ? Cast<ASPlayerController>(Instigator->Controller) : nullptr;
	FVector OutForward = FVector::ZeroVector;
	FRotator DummyRot;
	if (PC) {
		PC->GetPlayerViewPoint(OutForward, DummyRot);

		OutForward = DummyRot.Vector();
	}
	else if(Instigator){
		OutForward = OwnerPawn->GetBaseAimRotation().Vector();
	}
	return OutForward;
}

FVector ASWeapon::GetCameraDamageStartLocation(const FVector & AimDir) const
{
	ASPlayerController *PC = OwnerPawn ? Cast<ASPlayerController>(OwnerPawn->Controller) : nullptr;
	FVector OutStartTrace = FVector::ZeroVector;
	if (PC) {
		FRotator DummyRot;
		PC->GetPlayerViewPoint(OutStartTrace, DummyRot);

		FVector RawLocation = OwnerPawn->GetActorLocation();
		OutStartTrace += FVector::DotProduct(RawLocation - OutStartTrace, DummyRot.Vector()) * DummyRot.Vector();
	}

	return OutStartTrace;
}

FVector ASWeapon::GetMuzzleLocation() const
{
	return Mesh->GetSocketLocation(MuzzleAttachPoint);
}

FVector ASWeapon::GetMuzzleDirection() const
{
	return Mesh->GetSocketRotation(MuzzleAttachPoint).Vector();
}

FHitResult ASWeapon::WeaponTrace(const FVector & TraceFrom, const FVector & TraceTo) const
{
	FCollisionQueryParams TraceParams(TEXT("WeaponTrace"), true, Instigator);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;

	FHitResult Hit(ForceInit);

	GetWorld()->LineTraceSingleByChannel(Hit, TraceFrom, TraceTo, COLLISION_WEAPON, TraceParams);
	//DrawDebugLine(GetWorld(), TraceFrom, TraceTo, FColor::Red, true);

	return Hit;
}

void ASWeapon::ServerHandleFiring_Implementation()
{
	const bool bShouldUpdateAmmo = CanFire();
	HandleFiring();
	if (bShouldUpdateAmmo) {
		BurstCounter++;
	}
}

bool ASWeapon::ServerHandleFiring_Validate()
{
	return true;
}

void ASWeapon::GetLifetimeReplicatedProps(class TArray<class FLifetimeProperty, class FDefaultAllocator> &OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASWeapon, OwnerPawn);
	DOREPLIFETIME_CONDITION(ASWeapon, BurstCounter, COND_SkipOwner);
}