// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"
#include "SImpactEffect.generated.h"

UCLASS()
class MYSURVIVALGAME_API ASImpactEffect : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASImpactEffect();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UParticleSystem *GetImpactFX(EPhysicalSurface SurfaceType) const;
	USoundCue *GetImpactSound(EPhysicalSurface SurfaceType) const;

	virtual void PostInitializeComponents() override;

public:
	UPROPERTY(EditDefaultsOnly)
		UParticleSystem *DefaultFX;

	UPROPERTY(EditDefaultsOnly)
		UParticleSystem *FleshFX;
	
	UPROPERTY(EditDefaultsOnly)
		UParticleSystem *GreenFleshFX;

	UPROPERTY(EditDefaultsOnly)
		USoundCue *DefaultSound;

	UPROPERTY(EditDefaultsOnly)
		USoundCue *FleshSound;

	UPROPERTY(EditDefaultsOnly)
		USoundCue *GreenFleshSound;

	FHitResult SurfaceHit;
};
