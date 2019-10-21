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


// Sets default values
ASWeapon::ASWeapon(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer)
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

	CurrentState = NewState;
}

void ASWeapon::DetermineWeaponState()
{
	EWeaponState NewState = Idle;
	if (bIsEquiped) {
		NewState = Firing;
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

void ASWeapon::GetLifetimeReplicatedProps(class TArray<class FLifetimeProperty, class FDefaultAllocator> &OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASWeapon, OwnerPawn);
}