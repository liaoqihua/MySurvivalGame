// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include "SCharacter.generated.h"

UCLASS()
class MYSURVIVALGAME_API ASCharacter : public ACharacter
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
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
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
	UFUNCTION(BlueprintCallable, Category = PlayerCondition)
		float GetHealth() const;
	UFUNCTION(BlueprintCallable, Category = PlayerCondition)
		float GetMaxHealth() const;
	UFUNCTION(BlueprintCallable, Category = PlayerCondition)
		float GetHunger() const;
	UFUNCTION(BlueprintCallable, Category = PlayerCondition)
		float GetMaxHunger() const;
	UFUNCTION(BlueprintCallable, Category = PlayerCondition)
		void ConsumeFood(float AmountRestored);
	UFUNCTION(BlueprintCallable, Category = PlayerCondition)
		bool IsAlive() const;
public:
	void SetIsJumping(bool NewJumping);
	void SetIsSprinting(bool NewSprintint);
	class ASUsableActor *GetUsableView();
	void Use();
	void OnStartTargeting();
	void OnStopTargeting();
	void SetTargeting(bool NewTargeting);
	void IncrementHunger();

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

	//当前健康
	UPROPERTY(EditDefaultsOnly, Category = PlayerCondition, Replicated)
		float Health;

	//当前饥饿
	UPROPERTY(EditDefaultsOnly, Category = PlayerCondition, Replicated)
		float Hunger;

	//最大饥饿度
	UPROPERTY(EditDefaultsOnly, Category = PlayerCondition)
		float MaxHunger;

	//饥饿增长时间间隔
	UPROPERTY(EditDefaultsOnly, Category = PlayerCondition)
		float IncrementHungerInterval;

	//饥饿增长数值
	UPROPERTY(EditDefaultsOnly, Category = PlayerCondition)
		float IncrementHungerAmount;

	//饥饿临界值
	UPROPERTY(EditDefaultsOnly, Category = PlayerCondition)
		float CriticalHungerThreshold;

	//饥饿时掉血
	UPROPERTY(EditDefaultsOnly, Category = PlayerCondition)
		float HungerDamagePerInterval;

	UPROPERTY(EditDefaultsOnly, Category = PlayerCondition)
		TSubclassOf<UDamageType> HungerDamageType;
private:
	bool bHasNewFocus;
	UPROPERTY()
		class ASUsableActor *FocusedUsableActor;
};
