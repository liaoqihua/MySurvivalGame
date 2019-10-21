// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/SWeapon.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "SFlashlight.generated.h"

class USpotLightComponent;

/**
 * 
 */
UCLASS()
class MYSURVIVALGAME_API ASFlashlight : public ASWeapon
{
	GENERATED_UCLASS_BODY()
	
public:
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void OnUnEquip() override;

	virtual void OnEquipFinished() override;

	virtual void OnEquip() override;

	virtual void OnEnterInventory(ASCharacter *NewOwner) override;

	virtual void OnLeaveInventory() override;

public:
	void UpdateLight(bool Enabled);
	
private:
	UPROPERTY()
		UMaterialInstanceDynamic *MID;

	UPROPERTY(EditDefaultsOnly, Category = Material)
		FName EmissiveParamName;

	UPROPERTY(EditDefaultsOnly, Category = "Material")
		float MaxEmissiveIntensity;

	float LastEmissiveStrength;


public:
	UFUNCTION()
		void OnRep_IsActive(bool prevActive);

public:
	UPROPERTY(EditDefaultsOnly, Category = Material)
		FName LightAttachPoint;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_IsActive)
		bool bIsActive;

//×é¼þ
public:
	UPROPERTY(VisibleDefaultsOnly, Category = Components)
		UStaticMeshComponent *LightConeComp;

	UPROPERTY(VisibleDefaultsOnly, Category = Components)
		USpotLightComponent *SpotLightComp;

};
