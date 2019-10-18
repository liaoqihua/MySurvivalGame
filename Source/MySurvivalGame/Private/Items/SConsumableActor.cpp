// Fill out your copyright notice in the Description page of Project Settings.

#include "SConsumableActor.h"
#include "SCharacter.h"

ASConsumableActor::ASConsumableActor(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer), Nutrition(25.0f)
{

}

void ASConsumableActor::OnUsed(APawn *InstigatorPawn)
{
	Super::OnUsed(InstigatorPawn);

	ASCharacter *InstigatorCharacter = Cast<ASCharacter>(InstigatorPawn);
	if (InstigatorCharacter) {
		InstigatorCharacter->ConsumeFood(Nutrition);
	}

	Destroy();
}

