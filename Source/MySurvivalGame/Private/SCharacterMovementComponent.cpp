// Fill out your copyright notice in the Description page of Project Settings.

#include "SCharacterMovementComponent.h"
#include "SCharacter.h"




float USCharacterMovementComponent::GetMaxSpeed() const
{
	float MaxSpeed = Super::GetMaxSpeed();

	const ASCharacter *Character = Cast<ASCharacter>(GetOwner());
	if (Character) {
		if (Character->IsTargeting() && !Character->GetMovementComponent()->IsCrouching()) {
			MaxSpeed *= Character->GetTargetingSpeedScale();
		}
		else if (Character->IsInitiatedSprinting()) {
			MaxSpeed *= Character->GetSprintingSpeedScale();
		}
	}

	return MaxSpeed;
}
