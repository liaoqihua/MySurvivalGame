// Fill out your copyright notice in the Description page of Project Settings.

#include "SPlayerCameraManager.h"
#include "SCharacter.h"
#include <Camera/PlayerCameraManager.h>

ASPlayerCameraManager::ASPlayerCameraManager(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer), NormalFOV(90.0f), TargetFOV(65.0f)
{
	ViewPitchMin = -80.0f;
	ViewPitchMax = 87.0f;
	bAlwaysApplyModifiers = true;
}

void ASPlayerCameraManager::UpdateCamera(float DeltaTime)
{
	ASCharacter *Owner = PCOwner ? Cast<ASCharacter>(PCOwner->GetPawn()) : nullptr;
	if (Owner) {
		const float NowTargetFOV = Owner->IsTargeting() ? TargetFOV : NormalFOV;
		DefaultFOV = FMath::FInterpTo(DefaultFOV, NowTargetFOV, DeltaTime, 20.0f);
		SetFOV(DefaultFOV);
	}

	Super::UpdateCamera(DeltaTime);
}
