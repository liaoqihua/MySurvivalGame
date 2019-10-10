// Fill out your copyright notice in the Description page of Project Settings.

#include "SCharacter.h"
#include "SCharacterMovementComponent.h"
#include <GameFramework/PlayerInput.h>
#include <Engine/World.h>
#include <GameFramework/PlayerController.h>
#include <UnrealNetwork.h>


ASCharacter::ASCharacter(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<USCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true;

	CameraSprintArm = CreateDefaultSubobject<USpringArmComponent>("CameraSprintArm");
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	CameraSprintArm->SocketOffset = FVector(0, 35, 0);
	CameraSprintArm->TargetOffset = FVector(0, 0, 55);
	CameraSprintArm->bUsePawnControlRotation = true;
	CameraSprintArm->SetupAttachment(GetRootComponent());
	Camera->SetupAttachment(CameraSprintArm);

	UCharacterMovementComponent *MoveComp = GetCharacterMovement();
	if (MoveComp) {
		MoveComp->GravityScale = 1.5f;
		MoveComp->JumpZVelocity = 620;
		MoveComp->bCanWalkOffLedgesWhenCrouching = true;
		MoveComp->MaxWalkSpeedCrouched = 200;

		MoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;
	}

}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Lookup", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::OnStartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ASCharacter::OnStopJump);
	PlayerInputComponent->BindAction("CrouchToggle", IE_Released, this, &ASCharacter::OnCrouchToggle);
}

void ASCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode /* = 0 */)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	if (PrevMovementMode == MOVE_Falling && GetCharacterMovement()->MovementMode != MOVE_Falling) {
		SetIsJumping(false);
	}
}

void ASCharacter::MoveForward(float val)
{
	if (Controller && val) {
		const bool bLimitRotation = (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling());

		const FRotator Rotation = bLimitRotation ? GetActorRotation() : Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, val);
	}
}

void ASCharacter::MoveRight(float val)
{
	if (Controller && val) {
		const bool bLimitRotation = (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling());

		const FRotator Rotation = bLimitRotation ? GetActorRotation() : Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, val);
	}
}

void ASCharacter::OnStartJump()
{
	bPressedJump = true;

	SetIsJumping(true);
}

void ASCharacter::OnStopJump()
{
	bPressedJump = false;

}

bool ASCharacter::IsInitiatedJump() const
{
	return bIsJumping;
}

void ASCharacter::ServerSetJumping_Implementation(bool NewTargeting)
{
	SetIsJumping(NewTargeting);
}

bool ASCharacter::ServerSetJumping_Validate(bool NewTargeting)
{
	return true;
}

void ASCharacter::OnCrouchToggle()
{
	if (CanCrouch()) {
		Crouch();
	}
	else {
		UnCrouch();
	}
}

void ASCharacter::SetIsJumping(bool NewJumping)
{
	if (bIsCrouched && NewJumping) {
		UnCrouch();
	}else 
		bIsJumping = NewJumping;

	if (Role < ROLE_Authority) {
		ServerSetJumping(NewJumping);
	}
}

void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty, FDefaultAllocator> &OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//DOREPLIFETIME(ASCharacter, bIsJumping);
	DOREPLIFETIME_CONDITION( ASCharacter, bIsJumping, COND_SkipOwner);
}