// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/SPickUpActor.h"
#include "SConsumableActor.generated.h"

/**
 * 
 */
UCLASS()
class MYSURVIVALGAME_API ASConsumableActor : public ASPickUpActor
{
	GENERATED_UCLASS_BODY()
	
public:
	virtual void OnUsed(APawn *InstigatorPawn) override;

public:
	UPROPERTY(EditDefaultsOnly, Category = Consumable)
		float Nutrition;
};
