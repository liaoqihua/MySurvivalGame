// Fill out your copyright notice in the Description page of Project Settings.

#include "SBaseCharacter.h"
#include "UnrealNetwork.h"
#include "GameFramework/Controller.h"
#include <Components/CapsuleComponent.h>
#include "Components/PawnNoiseEmitterComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
ASBaseCharacter::ASBaseCharacter(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer), Health(100.0f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	NoiseEmitterComp = ObjectInitializer.CreateDefaultSubobject<UPawnNoiseEmitterComponent>(this, "NoiseEmitterComp");
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
}

// Called when the game starts or when spawned
void ASBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float ASBaseCharacter::GetHealth() const
{
	return Health;
}

float ASBaseCharacter::GetMaxHealth() const
{
	return GetClass()->GetDefaultObject<ASBaseCharacter>()->GetHealth();
}

float ASBaseCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Health <= 0.0f) return 0.0f;
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage) {
		Health = FMath::Clamp(Health - ActualDamage, 0.0f, GetMaxHealth());

		if (Health) {
			//Play Hit
			APawn *Pawn = EventInstigator ? EventInstigator->GetPawn() : nullptr;
			PlayHit(Damage, DamageEvent, Pawn, DamageCauser, false);
		}
		else {
			//Death
			Die(Damage, DamageEvent, EventInstigator, DamageCauser);
		}
	}
	return ActualDamage;
}

void ASBaseCharacter::PlayHit(float Damage, struct FDamageEvent const& DamageEvent, APawn* EventInstigator, AActor* DamageCauser, bool bKilled)
{
	if (Role == ROLE_Authority) ReplicateHit(Damage, DamageEvent, EventInstigator, DamageCauser, bKilled);

	if (Damage) ApplyDamageMomentum(Damage, DamageEvent, EventInstigator, DamageCauser);
}

bool ASBaseCharacter::Die(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!CanDie(Damage, DamageEvent, EventInstigator, DamageCauser))
		return false;
	else {
		const UDamageType *const DamageType = DamageEvent.DamageTypeClass ? DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>() : GetDefault<UDamageType>();
		//这个函数的作用在于保证EventInstigator不为nullptr;
		EventInstigator = GetDamageInstigator(EventInstigator, *DamageType);
		//这里在确定一下EventInstigator的情况，保证不会空指针
		OnDeath(Damage, DamageEvent, EventInstigator ? EventInstigator->GetPawn() : nullptr, DamageCauser);

		return true;
	}
}

bool ASBaseCharacter::CanDie(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const
{
	if (bIsDying || IsPendingKill() || Role != ROLE_Authority || !GetWorld()->GetAuthGameMode())
		return false;
	else return true;
}

void ASBaseCharacter::OnDeath(float Damage, struct FDamageEvent const& DamageEvent, APawn* EventInstigator, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("OnDeath"));

	//bIsDying为true直接返回
	if (bIsDying) return;

	PlayHit(Damage, DamageEvent, EventInstigator, DamageCauser, true);

	//取消移动复制，停止Actor更新到新客服端
	bReplicateMovement = false;
	TearOff();
	bIsDying = true;

	//将Controller上分离Pawn，并通知销毁
	DetachFromControllerPendingDestroy();

	//停止所有的AnimMontages
	//StopAllAnimMontages();

	//取消胶囊体的碰撞,忽略所有的通道
	UCapsuleComponent *CapsuleComp = GetCapsuleComponent();
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	//角色碰撞预设值设置为Ragdoll
	USkeletalMeshComponent *UseMesh = GetMesh();
	if (UseMesh) {
		UseMesh->SetCollisionProfileName(TEXT("Ragdoll"));
	}
	SetActorEnableCollision(true);
	SetRagdollPhysics();
}

void ASBaseCharacter::ReplicateHit(float Damage, struct FDamageEvent const& DamageEvent, APawn* EventInstigator, AActor* DamageCauser, bool bKilled)
{
	FDamageEvent const& LastDamageEvent = LastTakeHitInfo.GetDamageEvent();
	if (EventInstigator == LastTakeHitInfo.PawnInstigator.Get() && LastDamageEvent.DamageTypeClass == DamageEvent.DamageTypeClass) {
		if (bKilled && LastTakeHitInfo.bKilled) {
			return;
		}

		Damage += LastTakeHitInfo.ActualDamage;
	}

	LastTakeHitInfo.ActualDamage = Damage;
	LastTakeHitInfo.PawnInstigator = Cast<ASBaseCharacter>(EventInstigator);
	LastTakeHitInfo.SetDamageEvent(DamageEvent);
	LastTakeHitInfo.DamageCauser = DamageCauser;
	LastTakeHitInfo.bKilled = bKilled;
	LastTakeHitInfo.EnsureReplication();
}

void ASBaseCharacter::SetRagdollPhysics()
{
	//UE_LOG(LogTemp, Warning, TEXT("SetRagdollPhysics"));

	bool bInRagdoll = false;
	USkeletalMeshComponent *UseMesh = GetMesh();
	if (!IsPendingKill() && UseMesh && UseMesh->GetPhysicsAsset()) {
		bInRagdoll = true;
		UseMesh->SetAllBodiesSimulatePhysics(true);
		UseMesh->SetSimulatePhysics(true);
		UseMesh->WakeAllRigidBodies();
		UseMesh->bBlendPhysics = true;
	}

	UCharacterMovementComponent *CharacterComp = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (CharacterComp) {
		CharacterComp->StopMovementImmediately();
		CharacterComp->DisableMovement();
		CharacterComp->SetComponentTickEnabled(false);
	}

	if (!bInRagdoll) {
		TurnOff();
		SetActorHiddenInGame(true);
		SetLifeSpan(1.0f);
	}
	else SetLifeSpan(10.0f);
}

void ASBaseCharacter::OnRep_LastTakeHitInfo()
{
	if (LastTakeHitInfo.bKilled) {
		OnDeath(LastTakeHitInfo.ActualDamage, LastTakeHitInfo.GetDamageEvent(), LastTakeHitInfo.PawnInstigator.Get(), LastTakeHitInfo.DamageCauser.Get());
	}
	else {
		PlayHit(LastTakeHitInfo.ActualDamage, LastTakeHitInfo.GetDamageEvent(), LastTakeHitInfo.PawnInstigator.Get(), LastTakeHitInfo.DamageCauser.Get(), LastTakeHitInfo.bKilled);
	}
}

bool ASBaseCharacter::IsAlive() const
{
	return GetHealth() > 0.0f;
}
void ASBaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASBaseCharacter, Health);
	DOREPLIFETIME(ASBaseCharacter, LastTakeHitInfo);
}