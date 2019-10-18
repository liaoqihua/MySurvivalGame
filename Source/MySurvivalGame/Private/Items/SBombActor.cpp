// Fill out your copyright notice in the Description page of Project Settings.

#include "SBombActor.h"
#include <Particles/ParticleSystemComponent.h>
#include <UnrealNetwork.h>
#include <Components/AudioComponent.h>
#include <Engine/World.h>
#include <TimerManager.h>
#include <Sound/SoundCue.h>
#include <Kismet/GameplayStatics.h>

ASBombActor::ASBombActor(const FObjectInitializer &ObjectInitizlizer)
	:Super(ObjectInitizlizer), ExplosionFX(nullptr), FuzeFX(nullptr), ExplosionSound(nullptr), FuzeSound(nullptr),
	bIsFuzeActive(false), bExploded(false), MaxFuzeTime(5.0f), ExplosionDamage(100.0f), ExplosionRadius(512.0f)
{
	bReplicates = true;

	ExplosionPCS = ObjectInitizlizer.CreateDefaultSubobject<UParticleSystemComponent>(this, "ExplosionPCS");
	FuzePCS = ObjectInitizlizer.CreateDefaultSubobject<UParticleSystemComponent>(this, "FuzePCS");
	AudioComp = ObjectInitizlizer.CreateDefaultSubobject<UAudioComponent>(this, "AudioComp");

	ExplosionPCS->SetAutoActivate(false);
	FuzePCS->SetAutoActivate(false);
	AudioComp->SetAutoActivate(false);
	ExplosionPCS->bAutoDestroy = false;
	FuzePCS->bAutoDestroy = false;
	AudioComp->bAutoDestroy = false;

	ExplosionPCS->SetupAttachment(RootComponent);
	FuzePCS->SetupAttachment(RootComponent);
	AudioComp->SetupAttachment(RootComponent);
}

void ASBombActor::OnUsed(APawn *InstigatorPawn)
{
	Super::OnUsed(InstigatorPawn);

	if (!bIsFuzeActive) {
		bIsFuzeActive = true;

		SimulateFuzeFX();
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ASBombActor::OnExplode, MaxFuzeTime, false);
	}
}

void ASBombActor::OnRep_FuzeActive()
{
	//UE_LOG(LogTemp, Warning, TEXT("Modify bIsFuzeActive"));

	if (bIsFuzeActive && !bExploded) SimulateFuzeFX();
}

void ASBombActor::OnRep_ExplosionActive()
{
	//UE_LOG(LogTemp, Warning, TEXT("Modify bExploded"));

	if (bExploded)	SimulateExplosion();
}

void ASBombActor::OnExplode()
{
	//UE_LOG(LogTemp, Warning, TEXT("OnExplode"));

	if (bExploded) return;

	bExploded = true;
	SimulateExplosion();

	TArray<AActor*> IgnoreActors;
	UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), ExplosionRadius, DamageType, IgnoreActors, this, nullptr);
}

void ASBombActor::SimulateFuzeFX()
{
	//UE_LOG(LogTemp, Warning, TEXT("SimulateFuzeFX"));

	if (FuzeSound) {
		AudioComp->SetSound(FuzeSound);
		AudioComp->FadeIn(0.25f);
	}

	if (FuzeFX) {
		FuzePCS->SetTemplate(FuzeFX);
		FuzePCS->Activate();
	}
}

void ASBombActor::SimulateExplosion()
{
	//UE_LOG(LogTemp, Warning, TEXT("SimulateExplosion"));

	MeshComp->SetVisibility(false);

	if (FuzeSound) {
		if (ExplosionSound) {
			AudioComp->SetSound(ExplosionSound);
		}
		else {
			AudioComp->Stop();
		}
	}

	if (FuzeFX)	FuzePCS->Deactivate();

	if (ExplosionFX) {
		ExplosionPCS->SetTemplate(ExplosionFX);
		ExplosionPCS->Activate();
	}
}

void ASBombActor::GetLifetimeReplicatedProps(class TArray<class FLifetimeProperty, class FDefaultAllocator> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASBombActor, bIsFuzeActive);
	DOREPLIFETIME(ASBombActor, bExploded);
}