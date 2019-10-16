// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Stypes.h"
#include "SWeapon.generated.h"

UENUM()
enum EWeaponState
{
	Idle,
	Firing,
	Equipping,
	Reloading
};

UCLASS()
class MYSURVIVALGAME_API ASWeapon : public AActor
{
	GENERATED_UCLASS_BODY()
	
public:	
	// Sets default values for this actor's properties
	//ASWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditDefaultsOnly, Category = Components)
		USkeletalMeshComponent *Mesh;

public:
	EWeaponState GetCurrentState() const;
	FORCEINLINE EInventorySlot GetStorageSlot()
	{
		return StorageSlot;
	}

private:
	EWeaponState CurrentState;
	EInventorySlot StorageSlot;
};
