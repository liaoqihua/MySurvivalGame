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

protected:
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

//武器开火
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

	//循环射击函数
	virtual void HandleFiring();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerHandleFiring();

	//开火相关效果函数
	virtual void SimulateWeaponFire();
	virtual void StopSimulateWeaponFire();

private:
	bool bWantsToFire;

protected:
	//射击间隔
	UPROPERTY(EditDefaultsOnly)
		float TimeBetweenShots;

	//上次射击时间
	float LastFireTime;

	//是否可以再次开火
	bool bRefiring;

	//开火子弹计数，网络复制，停止开火清空
	UPROPERTY(Transient, ReplicatedUsing = OnRep_BurstCounter)
		int32 BurstCounter;

	UFUNCTION()
		void OnRep_BurstCounter();

//开火特效
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

	//获得瞄准的方向
	FVector GetAdjustedAim() const;

	//获得摄像机前位置
	FVector GetCameraDamageStartLocation(const FVector &AimDir) const;

	//获得枪口粒子的位置
	FVector GetMuzzleLocation() const;

	//获得枪口粒子方向
	FVector GetMuzzleDirection() const;

	//画射线
	FHitResult WeaponTrace(const FVector &TraceFrom, const FVector &TraceTo) const;
};
