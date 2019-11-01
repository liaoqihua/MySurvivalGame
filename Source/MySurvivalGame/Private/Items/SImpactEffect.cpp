// Fill out your copyright notice in the Description page of Project Settings.

#include "SImpactEffect.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASImpactEffect::ASImpactEffect()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASImpactEffect::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASImpactEffect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UParticleSystem * ASImpactEffect::GetImpactFX(EPhysicalSurface SurfaceType) const
{
	switch (SurfaceType)
	{
	case SurfaceType_Default:
		return DefaultFX;
	case SurfaceType1:
		return FleshFX;
	case SurfaceType2:
		return GreenFleshFX;
	default:
		return nullptr;
	}
}

USoundCue * ASImpactEffect::GetImpactSound(EPhysicalSurface SurfaceType) const
{
	switch (SurfaceType)
	{
	case SurfaceType_Default:
		return DefaultSound;
	case SurfaceType1:
		return FleshSound;
	case SurfaceType2:
		return GreenFleshSound;
	default:
		return nullptr;
	}
}

void ASImpactEffect::PostInitializeComponents()
{
	UPhysicalMaterial *HitPhysicalMat = SurfaceHit.PhysMaterial.Get();
	EPhysicalSurface HitSurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitPhysicalMat);
	UParticleSystem *ImpactFX = GetImpactFX(HitSurfaceType);
	if (ImpactFX) {
		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactFX, GetActorLocation(), GetActorRotation());
	}
	USoundCue *ImpactSound = GetImpactSound(HitSurfaceType);
	if (ImpactSound) {
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}

	Super::PostInitializeComponents();
}

