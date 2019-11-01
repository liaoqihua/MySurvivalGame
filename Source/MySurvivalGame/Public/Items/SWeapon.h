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

	UFUNCTION(BlueprintCallable, Category = Weapon)
		USkeletalMeshComponent *GetWeaponMesh() const;

	UFUNCTION(BlueprintCallable, Category = Weapon)
		ASCharacter *GetPawnOwner() const;

public:
	UPROPERTY(VisibleDefaultsOnly, Category = Components)
		USkeletalMeshComponent *Mesh;

public:
	EWeaponState GetCurrentState() const;
	FORCEINLINE EInventorySlot GetStorageSlot() const
	{
		return StorageSlot;
	}

	//���뱳���󸽼ӵ����ص�
	virtual void OnEnterInventory(ASCharacter *NewOwner);

	//����Owner
	void SetOwningPawn(ASCharacter *NewOwner);

	//�������ӵ���ɫ���ص㣬Ĭ�Ϲ���������
	void AttachMeshToPawn(EInventorySlot Slot = EInventorySlot::Hands);

	//������ж������
	void DetachMeshFromPawn();

public:
	UFUNCTION()
		void OnRep_OwnerPawn();

	UPROPERTY(Transient, ReplicatedUsing = OnRep_OwnerPawn)
		ASCharacter *OwnerPawn;

protected:
	EWeaponState CurrentState;
	EInventorySlot StorageSlot;

public:
	//װ������
	virtual void OnEquip();

	//ж������
	virtual void OnUnEquip();

	virtual void OnEquipFinished();
	
	UAudioComponent *PlayWeaponSound(USoundCue *SoundToPlay);

	void SetWeaponState(EWeaponState NewState);

	//ȷ����ǰ����״̬
	void DetermineWeaponState();

	float PlayWeaponAnimation(UAnimMontage *Animation, float InPlayRate = 1.f, FName StartSectionName = NAME_None);

	void StopWeaponAnimation(UAnimMontage *Animation);

	virtual void OnLeaveInventory();

	bool IsEquiped() const;

	bool IsAttachedToPawn() const;
public:
	//�ϴ������л�ʱ��
	float EquipStartedTime;

	//����װ������ʱ��
	float EquipDuration;

	//�Ƿ�װ��
	bool bIsEquiped;

	//�Ƿ���װ��������
	bool bPendingEquip;

	FTimerHandle FiringTimerHandle;
	FTimerHandle EquipFinishedTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = EquipSounds)
		USoundCue *EquipSound;

	UPROPERTY(EditDefaultsOnly, Category = EquipAnim)
		UAnimMontage *EquipAnim;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<ASWeaponPickUp> WeaponPickupClass;

//��������
public:
	void StartFire();

	void StopFire();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerStartFire();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerStopFire();

	bool CanFire() const;

	void OnBurstStarted();

	void OnBurstFinished();

	//ѭ���������
	virtual void HandleFiring();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerHandleFiring();

	//�������Ч������
	virtual void SimulateWeaponFire();
	virtual void StopSimulateWeaponFire();

private:
	bool bWantsToFire;

protected:
	//������
	UPROPERTY(EditDefaultsOnly)
		float TimeBetweenShots;

	//�ϴ����ʱ��
	float LastFireTime;

	//�Ƿ�����ٴο���
	bool bRefiring;

	//�����ӵ����������縴�ƣ�ֹͣ�������
	UPROPERTY(Transient, ReplicatedUsing = OnRep_BurstCounter)
		int32 BurstCounter;

	UFUNCTION()
		void OnRep_BurstCounter();

//������Ч
public:
	UPROPERTY(EditDefaultsOnly, Category = "SimulationFire")
		USoundCue *FireSound;

	UPROPERTY(EditDefaultsOnly, Category = "SimulationFire")
		UParticleSystem *MuzzleFX;

	UPROPERTY(EditDefaultsOnly, Category = "SimulationFire")
		FName MuzzleAttachPoint;

	UPROPERTY(Transient)
		UParticleSystemComponent *MuzzlePSC;

	UPROPERTY(EditDefaultsOnly, Category = "SimulationFire")
		UAnimMontage *FireAnim;

	bool bPlayingFireAnim;

protected:
	virtual void FireWeapon() PURE_VIRTUAL(ASWeapon::FireWeapon(), );

	//�����׼�ķ���
	FVector GetAdjustedAim() const;

	//��������ǰλ��
	FVector GetCameraDamageStartLocation(const FVector &AimDir) const;

	//���ǹ�����ӵ�λ��
	FVector GetMuzzleLocation() const;

	//���ǹ�����ӷ���
	FVector GetMuzzleDirection() const;

	//������
	FHitResult WeaponTrace(const FVector &TraceFrom, const FVector &TraceTo) const;
};
