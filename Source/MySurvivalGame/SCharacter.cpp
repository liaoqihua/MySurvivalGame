// Fill out your copyright notice in the Description page of Project Settings.

#include "SCharacter.h"
#include "SCharacterMovementComponent.h"
#include <GameFramework/PlayerInput.h>
#include <Engine/World.h>
#include <GameFramework/PlayerController.h>


ASCharacter::ASCharacter(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<USCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

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

	//FInputActionKeyMapping UseKey("Use", EKeys::E);
	//GetWorld()->GetFirstPlayerController()->PlayerInput->AddActionMapping(UseKey);
	//FInputActionKeyMapping JumpKey("Jump", EKeys::SpaceBar);
	//GetWorld()->GetFirstPlayerController()->PlayerInput->AddActionMapping(JumpKey);
	//FInputActionKeyMapping FireKey("Fire", EKeys::LeftMouseButton);
	//GetWorld()->GetFirstPlayerController()->PlayerInput->AddActionMapping(FireKey);
	//FInputActionKeyMapping TargetingKey("Targeting", EKeys::RightMouseButton);
	//GetWorld()->GetFirstPlayerController()->PlayerInput->AddActionMapping(TargetingKey);
	//FInputActionKeyMapping SprintHoldKey("SprintHold", EKeys::LeftShift);
	//GetWorld()->GetFirstPlayerController()->PlayerInput->AddActionMapping(SprintHoldKey);
	//FInputActionKeyMapping CrouchToggleKey("CrouchToggle", EKeys::LeftControl);
	//GetWorld()->GetFirstPlayerController()->PlayerInput->AddActionMapping(CrouchToggleKey);
	//FInputActionKeyMapping CrouchToggleKey_1("CrouchToggle", EKeys::C);
	//GetWorld()->GetFirstPlayerController()->PlayerInput->AddActionMapping(CrouchToggleKey_1);

	//FInputAxisKeyMapping ForwardKey("MoveForward", EKeys::W, 1.0f);
	//FInputAxisKeyMapping BackKey("MoveForward", EKeys::S, -1.0f);
	//GetWorld()->GetFirstPlayerController()->PlayerInput->AddAxisMapping(ForwardKey);
	//GetWorld()->GetFirstPlayerController()->PlayerInput->AddAxisMapping(BackKey);
	//FInputAxisKeyMapping RightKey("MoveRight", EKeys::D, 1.0f);
	//FInputAxisKeyMapping LeftKey("MoveRight", EKeys::A, -1.0f);
	//GetWorld()->GetFirstPlayerController()->PlayerInput->AddAxisMapping(RightKey);
	//GetWorld()->GetFirstPlayerController()->PlayerInput->AddAxisMapping(LeftKey);
	//FInputAxisKeyMapping TurnKey("Turn", EKeys::MouseX, 1.0f);
	//GetWorld()->GetFirstPlayerController()->PlayerInput->AddAxisMapping(TurnKey);
	//FInputAxisKeyMapping LookupKey("Lookup", EKeys::MouseY, -1.0f);
	//GetWorld()->GetFirstPlayerController()->PlayerInput->AddAxisMapping(LookupKey);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Lookup", this, &APawn::AddControllerPitchInput);
}

void ASCharacter::MoveForward(float val)
{
	if (Controller && val) {
		const bool bLimitRotation = (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling());

		const FRotator Rotation = bLimitRotation ? GetActorRotation() : Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetUnitAxis(EAxis::X);
		//const FVector Direction = GetActorForwardVector();
		AddMovementInput(Direction, val);
	}
}

void ASCharacter::MoveRight(float val)
{
	if (Controller && val) {
		const bool bLimitRotation = (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling());

		const FRotator Rotation = bLimitRotation ? GetActorRotation() : Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetUnitAxis(EAxis::Y);
		//const FVector Direction = GetActorRightVector();
		AddMovementInput(Direction, val);
	}
}