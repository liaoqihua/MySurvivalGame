// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SUsableActor.h"
#include "SPickUpActor.generated.h"

/**
 * 
 */
UCLASS()
class MYSURVIVALGAME_API ASPickUpActor : public ASUsableActor
{
	GENERATED_UCLASS_BODY()
	
public:
	virtual void OnUsed(APawn *InstigatorPawn) override;

public:
	UPROPERTY(EditDefaultsOnly, Category = Sound)
		class USoundCue *PickUpSound;
};
