// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/SWeapon.h"
#include "SWeaponInstant.generated.h"

/**
 * 
 */
UCLASS()
class MYSURVIVALGAME_API ASWeaponInstant : public ASWeapon
{
	GENERATED_BODY()
	
public:
	virtual void FireWeapon() override;
	
	
};
