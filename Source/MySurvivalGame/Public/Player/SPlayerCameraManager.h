// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "SPlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class MYSURVIVALGAME_API ASPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_UCLASS_BODY()

public:
	virtual void UpdateCamera(float DeltaTime) override;
	
public:
	float NormalFOV;
	float TargetFOV;
};
