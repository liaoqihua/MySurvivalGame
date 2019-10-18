// Fill out your copyright notice in the Description page of Project Settings.

#include "SWeaponPickUp.h"
#include "SCharacter.h"
#include "SWeapon.h"
#include <Class.h>
#include <Engine/World.h>

ASWeaponPickUp::ASWeaponPickUp(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer)
{
	
}

void ASWeaponPickUp::OnUsed(APawn *InstigatorPawn)
{
	Super::OnUsed(InstigatorPawn);

	ASCharacter *OwnerPawn = Cast<ASCharacter>(InstigatorPawn);
	if (OwnerPawn && WeaponClass && Role == ROLE_Authority) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Use Weapon Pickup");
		if (OwnerPawn->WeaponSlotAvailable(WeaponClass->GetDefaultObject<ASWeapon>()->GetStorageSlot())) {
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			ASWeapon *Weapon = GetWorld()->SpawnActor<ASWeapon>(WeaponClass, SpawnInfo);

			OwnerPawn->AddWeapon(Weapon);

			Destroy();
		}
	}
}

