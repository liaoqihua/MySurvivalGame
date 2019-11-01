// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
//#include "Stypes.h"
#include "SBaseCharacter.h"
#include "SCharacter.generated.h"

class ASWeapon;

UCLASS()
class MYSURVIVALGAME_API ASCharacter : public ASBaseCharacter
{
	GENERATED_UCLASS_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode /* = 0 */) override;
	//virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	//virtual bool CanDie(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const;
	//virtual bool Die(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);
	virtual void OnDeath(float Damage, struct FDamageEvent const& DamageEvent, APawn* EventInstigator, AActor* DamageCauser) override;
	//virtual void PlayHit(float Damage, struct FDamageEvent const& DamageEvent, APawn* EventInstigator, AActor* DamageCauser, bool bKilled);
public:
	UFUNCTION()
		void MoveForward(float val);
	UFUNCTION()
		void MoveRight(float val);
	UFUNCTION()
		void OnStartJump();
	UFUNCTION()
		void OnStopJump();
	UFUNCTION(BlueprintCallable, Category = Movement)
		bool IsInitiatedJump() const;
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSetJumping(bool NewTargeting);
	UFUNCTION()
		void OnCrouchToggle();
	UFUNCTION()
		void OnStartSprinting();
	UFUNCTION()
		void OnStopSprinting();
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSetSprinting(bool NewTargeting);
	UFUNCTION(BlueprintCallable, Category = Movement)
		bool IsInitiatedSprinting() const;
	UFUNCTION(BlueprintCallable, Category = Movement)
		float GetSprintingSpeedScale() const;
	UFUNCTION(BlueprintCallable, Category = Movement)
		float GetTargetingSpeedScale() const;
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerUse();
	UFUNCTION(BlueprintCallable, Category = Targeting)
		bool IsTargeting() const;
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSetTargeting(bool NewTargeting);
	UFUNCTION(BlueprintCallable, Category = Targeting)
		FRotator GetAimOffsets() const;
	//UFUNCTION(BlueprintCallable, Category = PlayerCondition)
	//	float GetHealth() const;
	//UFUNCTION(BlueprintCallable, Category = PlayerCondition)
	//	float GetMaxHealth() const;
	UFUNCTION(BlueprintCallable, Category = PlayerCondition)
		float GetHunger() const;
	UFUNCTION(BlueprintCallable, Category = PlayerCondition)
		float GetMaxHunger() const;
	UFUNCTION(BlueprintCallable, Category = PlayerCondition)
		void ConsumeFood(float AmountRestored);
	//UFUNCTION(BlueprintCallable, Category = PlayerCondition)
	//	bool IsAlive() const;
	//UFUNCTION()
	//	virtual void ReplicateHit(float Damage, struct FDamageEvent const& DamageEvent, APawn* EventInstigator, AActor* DamageCauser, bool bKilled);
	//UFUNCTION()
	//	void OnRep_LastTakeHitInfo();
public:
	void SetIsJumping(bool NewJumping);
	void SetIsSprinting(bool NewSprintint);
	class ASUsableActor *GetUsableView();
	void Use();
	void OnStartTargeting();
	void OnStopTargeting();
	void SetTargeting(bool NewTargeting);
	void IncrementHunger();
	//void SetRagdollPhysics();
	void StopAllAnimMontages();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		USpringArmComponent *CameraSprintArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		UCameraComponent *Camera;
	UPROPERTY(Transient, Replicated)
		bool bIsJumping;
	UPROPERTY(Transient, Replicated)
		bool bIsSprinting;
	UPROPERTY(EditDefaultsOnly, Category = Movement)
		float SprintingSpeedScale;
	UPROPERTY(EditDefaultsOnly, Category = Movement)
		float TargetingSpeedScale;
	UPROPERTY(EditDefaultsOnly, Category = ObjectInteraction)
		float MaxUseDistance;
	UPROPERTY(Transient, Replicated)
		bool bIsTargeting;

	////��ǰ����
	//UPROPERTY(EditDefaultsOnly, Category = PlayerCondition, Replicated)
	//	float Health;

	//��ǰ����
	UPROPERTY(EditDefaultsOnly, Category = PlayerCondition, Replicated)
		float Hunger;

	//��󼢶���
	UPROPERTY(EditDefaultsOnly, Category = PlayerCondition)
		float MaxHunger;

	//��������ʱ����
	UPROPERTY(EditDefaultsOnly, Category = PlayerCondition)
		float IncrementHungerInterval;

	//����������ֵ
	UPROPERTY(EditDefaultsOnly, Category = PlayerCondition)
		float IncrementHungerAmount;

	//�����ٽ�ֵ
	UPROPERTY(EditDefaultsOnly, Category = PlayerCondition)
		float CriticalHungerThreshold;

	//����ʱ��Ѫ
	UPROPERTY(EditDefaultsOnly, Category = PlayerCondition)
		float HungerDamagePerInterval;

	UPROPERTY(EditDefaultsOnly, Category = PlayerCondition)
		TSubclassOf<UDamageType> HungerDamageType;

	//�Ƿ�����
	//bool bIsDying;

	//UPROPERTY(Transient, ReplicatedUsing = OnRep_LastTakeHitInfo)
	//	struct FTakeHitInfo LastTakeHitInfo;

private:
	bool bHasNewFocus;
	UPROPERTY()
		class ASUsableActor *FocusedUsableActor;

public:
	//��ʼ���
	UFUNCTION()
		void OnStartFire();

	//ֹͣ���
	UFUNCTION()
		void OnStopFire();

	//�л���һ������
	UFUNCTION()
		void OnNextWeapon();

	//�л�ǰһ������
	UFUNCTION()
		void OnPrevWeapon();

	//װ��������
	UFUNCTION()
		void OnEquipPrimaryWeapon();

	//װ��������
	UFUNCTION()
		void OnEquipSecondWeapon();

	//��ʼ���
		void StartWeaponFire();
	//ֹͣ���
		void StopWeaponFire();
	//��ձ���
		void DestroyInventory();

	//��������
	UFUNCTION()
		void DropWeapon();
	//��������������
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerDropWeapon();

	//�޸�CurrentWeapon����õĺ���
	UFUNCTION()
		void OnRep_CurrentWeapon(ASWeapon *LastWeapon);

	//����ض���Slot�Ƿ���Ч���޶�һ������һ������
	bool WeaponSlotAvailable(EInventorySlot CheckSlot);

	//�ܷ񿪻�
	bool CanFire() const;
	
	//�Ƿ��ڿ���
	UFUNCTION(BlueprintCallable, Category = Weapon)
		bool IsFiring() const;

	//���ر������ص�����
	FName GetInventoryAttachPoint(EInventorySlot Slot);

	//���õ�ǰʹ�õ�����
	void SetCurrentWeapon(ASWeapon *NewWeapon, ASWeapon *LastWeapon = nullptr);

	//װ������
	void EquipWeapon(ASWeapon *NewWeapon);

	//����װ������
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerEquipWeapon(ASWeapon *NewWeapon);

	//�򱳰����һ������
	void AddWeapon(ASWeapon *Weapon);

	//�ӱ���ɾ��һ������
	void RemoveWeapon(ASWeapon *Weapon);

private:
	UPROPERTY(EditDefaultsOnly, Category = Sockets)
		FName WeaponAttachPoint;
	UPROPERTY(EditDefaultsOnly, Category = Sockets)
		FName PelvisAttachPoint;
	UPROPERTY(EditDefaultsOnly, Category = Sockets)
		FName SpineAttachPoint;
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
		float DropItemDistance;

	bool bWantsToFire;

	UPROPERTY(Transient, Replicated)
		TArray<ASWeapon*> Inventory;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_CurrentWeapon)
		ASWeapon *CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, Category = Inventory)
		TArray<TSubclassOf<ASWeapon>> DefaultInventoryClasses;

	void InitDefaultInventory();

public:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void PawnClientRestart() override;

	UFUNCTION(BlueprintCallable, Category = "AI")
		void MakePawnNoise(float Loudness);

private:
	float LastNoiseLoudness;

	float LastMakeNoiseTime;

public:
	UFUNCTION(BlueprintCallable, Category = "AI")
		float GetLastNoiseLoudness() const;

	UFUNCTION(BlueprintCallable, Category = "AI")
		float GetLastMakeNoiseTime() const;
};
