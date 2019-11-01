// Fill out your copyright notice in the Description page of Project Settings.

#include "SZombieCharacter.h"
#include "Perception/PawnSensingComponent.h"
#include "MySurvivalGame.h"
#include <Components/CapsuleComponent.h>
#include "SZombieAIController.h"

ASZombieCharacter::ASZombieCharacter(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer)
{
	PawnSensingComp = ObjectInitializer.CreateDefaultSubobject<UPawnSensingComponent>(this, "PawnSensingComp");

	PawnSensingComp->SetPeripheralVisionAngle(60.0f);
	PawnSensingComp->SightRadius = 2000.0f;
	PawnSensingComp->HearingThreshold = 600.0f;
	PawnSensingComp->LOSHearingThreshold = 1200.0f;

	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);
	GetCapsuleComponent()->SetCapsuleHalfHeight(96.0f, false);
	GetCapsuleComponent()->SetCapsuleRadius(42.0f);

	Health = 75.0f;
	PunchDamage = 10.0f;

	BotType = EBotBehaviorType::Passive;
	SenseTimeOut = 2.5f;

	bSenseTarget = false;
}

void ASZombieCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (PawnSensingComp) {
		PawnSensingComp->OnSeePawn.AddDynamic(this, &ASZombieCharacter::OnSeePlayer);
		PawnSensingComp->OnHearNoise.AddDynamic(this, &ASZombieCharacter::OnHearNoise);
	}
}

void ASZombieCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bSenseTarget && (GetWorld()->TimeSeconds - LastSeenTime) > SenseTimeOut && (GetWorld()->TimeSeconds - LastHeardTime) > SenseTimeOut)
	{
		ASZombieAIController* AIController = Cast<ASZombieAIController>(GetController());
		if (AIController)
		{
			bSenseTarget = false;
			/* Reset */
			AIController->SetTargetEnemy(nullptr);
		}
	}
}

void ASZombieCharacter::OnSeePlayer(APawn * Pawn)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "OnSeePlayer");
	LastSeenTime = GetWorld()->GetTimeSeconds();
	bSenseTarget = true;

	ASZombieAIController *AIController = Cast<ASZombieAIController>(Controller);
	ASBaseCharacter *SensedPawn = Cast<ASBaseCharacter>(Pawn);
	if (AIController && SensedPawn->IsAlive()) {
		AIController->SetMoveToTarget(Pawn);
	}
}

void ASZombieCharacter::OnHearNoise(APawn * MyInstigator, const FVector & Location, float Volume)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "OnHearNoise");

	bSenseTarget = true;
	LastHeardTime = GetWorld()->GetTimeSeconds();

	//DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), Location, 64, FColor::Green, false, 0.2f);

	ASZombieAIController* AIController = Cast<ASZombieAIController>(GetController());
	if (AIController)
	{
		AIController->SetMoveToTarget(MyInstigator);
	}

}