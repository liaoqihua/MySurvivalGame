// Fill out your copyright notice in the Description page of Project Settings.

#include "SPickUpActor.h"
#include <Sound/SoundCue.h>
#include <Kismet/GameplayStatics.h>

ASPickUpActor::ASPickUpActor(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer)
{
	MeshComp->SetSimulatePhysics(true);

	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bReplicateMovement = true;
}

void ASPickUpActor::OnUsed(APawn *InstigatorPawn)
{
	Super::OnUsed(InstigatorPawn);

	if (PickUpSound) {
		UGameplayStatics::PlaySoundAtLocation(this, PickUpSound, GetActorLocation());
	}
}

