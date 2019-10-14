// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/SPickUpActor.h"
#include "SBombActor.generated.h"

class UParticleSystemComponent;
class UAudioComponent;

/**
 * 
 */
UCLASS()
class MYSURVIVALGAME_API ASBombActor : public ASPickUpActor
{
	GENERATED_UCLASS_BODY()
	
public:
	virtual void OnUsed(APawn *InstigatorPawn) override;

public:
	UFUNCTION()
		void OnRep_FuzeActive();
	UFUNCTION()
		void OnRep_ExplosionActive();
	UFUNCTION()
		void OnExplode();
	//µãÈ¼µ¼»ðË÷
	UFUNCTION()
		void SimulateFuzeFX();
	//±¬Õ¨
	UFUNCTION()
		void SimulateExplosion();
public:
	UPROPERTY(VisibleDefaultsOnly, Category = Compoents)
		UParticleSystemComponent *ExplosionPCS;
	UPROPERTY(VisibleDefaultsOnly, Category = Compoents)
		UParticleSystemComponent *FuzePCS;
	UPROPERTY(VisibleDefaultsOnly, Category = Compoents)
		UAudioComponent *AudioComp;
	UPROPERTY(EditAnywhere, Category = "Bomb|Effects")
		UParticleSystem *ExplosionFX;
	UPROPERTY(EditAnywhere, Category = "Bomb|Effects")
		UParticleSystem *FuzeFX;
	UPROPERTY(EditAnywhere, Category = "Bomb|Sounds")
		USoundCue *ExplosionSound;
	UPROPERTY(EditAnywhere, Category = "Bomb|Sounds")
		USoundCue *FuzeSound;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_FuzeActive)
		bool bIsFuzeActive;
	UPROPERTY(Transient, ReplicatedUsing = OnRep_ExplosionActive)
		bool bExploded;
	UPROPERTY(EditDefaultsOnly, Category = "Bomb|Props")
		float MaxFuzeTime;
	UPROPERTY(EditDefaultsOnly, Category = "Bomb|Props")
		float ExplosionDamage;
	UPROPERTY(EditDefaultsOnly, Category = "Bomb|Props")
		float ExplosionRadius;
	UPROPERTY(EditDefaultsOnly, Category = "Bomb|Props")
		TSubclassOf<UDamageType> DamageType;
};
