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

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode /* = 0 */);
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

public:
	void SetIsJumping(bool NewJumping);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		USpringArmComponent *CameraSprintArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		UCameraComponent *Camera;
	UPROPERTY(Transient, Replicated)
		bool bIsJumping;
};
