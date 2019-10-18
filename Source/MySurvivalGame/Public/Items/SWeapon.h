// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Stypes.h"
#include "SWeapon.generated.h"

class UAudioComponent;
class USoundCue;
class UAnimMontage;
class ASWeaponPickUp;

UENUM()
enum EWeaponState
{
	Idle,
	Firing,
	Equipping,
	Reloading
};

UCLASS()
class MYSURVIVALGAME_API ASWeapon : public AActor
{
	GENERATED_UCLASS_BODY()
	
public:	
	// Sets default values for this actor's properties
	//ASWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleDefaultsOnly, Category = Components)
		USkeletalMeshComponent *Mesh;

public:
	EWeaponState GetCurrentState() const;
	FORCEINLINE EInventorySlot GetStorageSlot() const
	{
		return StorageSlot;
	}

	//进入背包后附加到挂载点
	virtual void OnEnterInventory(ASCharacter *NewOwner);

	//设置Owner
	void SetOwningPawn(ASCharacter *NewOwner);

	//武器附加到角色挂载点，默认挂载在手上
	void AttachMeshToPawn(EInventorySlot Slot = EInventorySlot::Hands);

	//将武器卸载下来
	void DetachMeshFromPawn();

public:
	UFUNCTION()
		void OnRep_OwnerPawn();

	UPROPERTY(Transient, ReplicatedUsing = OnRep_OwnerPawn)
		ASCharacter *OwnerPawn;

private:
	EWeaponState CurrentState;
	EInventorySlot StorageSlot;

public:
	//装备武器
	virtual void OnEquip();

	//卸载武器
	virtual void OnUnEquip();

	virtual void OnEquipFinished();
	
	UAudioComponent *PlayWeaponSound(USoundCue *SoundToPlay);

	void SetWeaponState(EWeaponState NewState);

	//确定当前武器状态
	void DetermineWeaponState();

	float PlayWeaponAnimation(UAnimMontage *Animation, float InPlayRate = 1.f, FName StartSectionName = NAME_None);

	void StopWeaponAnimation(UAnimMontage *Animation);

	virtual void OnLeaveInventory();

	bool IsEquiped() const;

	bool IsAttachedToPawn() const;
public:
	//上次武器切换时间
	float EquipStartedTime;

	//武器装备花费时间
	float EquipDuration;

	//是否装备
	bool bIsEquiped;

	//是否在装备过程中
	bool bPendingEquip;

	FTimerHandle FiringTimerHandle;
	FTimerHandle EquipFinishedTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = EquipSounds)
		USoundCue *EquipSound;

	UPROPERTY(EditDefaultsOnly, Category = EquipAnim)
		UAnimMontage *EquipAnim;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<ASWeaponPickUp> WeaponPickupClass;
};
