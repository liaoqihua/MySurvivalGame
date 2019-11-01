// Fill out your copyright notice in the Description page of Project Settings.

#include "SZombieAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SZombieCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "SBotWayPoint.h"

ASZombieAIController::ASZombieAIController(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer)
{
	BehaviorComp = ObjectInitializer.CreateDefaultSubobject<UBehaviorTreeComponent>(this, "BehaviorComp");
	BlackboardComp = ObjectInitializer.CreateDefaultSubobject<UBlackboardComponent>(this, "BlackboardComp");

	TargetLocationKeyName = "TargetLocation";
	PatrolLocationKeyName = "PatrolLocation";
	CurrentWayPointKeyName = "CurrentWaypoint";
	BotTypeKeyName = "BotType";
	TargetEnemyKeyName = "TargetEnemy";

	bWantsPlayerState = true;
}

void ASZombieAIController::Possess(APawn * InPawn)
{
	Super::Possess(InPawn);

	ASZombieCharacter* ZombieBot = Cast<ASZombieCharacter>(InPawn);
	if (ZombieBot) {
		if (ZombieBot->BehaviorTree) {
			if (ZombieBot->BehaviorTree->BlackboardAsset) {
				BlackboardComp->InitializeBlackboard(*ZombieBot->BehaviorTree->BlackboardAsset);
				BlackboardComp->SetValueAsEnum(BotTypeKeyName, (uint8)ZombieBot->BotType);
			}
			BehaviorComp->StartTree(*ZombieBot->BehaviorTree);
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("ZombieBot BehaviorTree is NULL"));
		}
	}
}

ASBotWayPoint * ASZombieAIController::GetWaypoint()
{
	if (BlackboardComp) {
		return Cast<ASBotWayPoint>(BlackboardComp->GetValueAsObject(CurrentWayPointKeyName));
	}
	return nullptr;
}

ASBaseCharacter * ASZombieAIController::GetTargetEnemy()
{
	if (BlackboardComp) {
		return Cast<ASBaseCharacter>(BlackboardComp->GetValueAsObject(TargetEnemyKeyName));
	}
	return nullptr;
}

void ASZombieAIController::SetMoveToTarget(APawn * InPawn)
{
	if (BlackboardComp) {
		SetTargetEnemy(InPawn);

		if (InPawn) {
			BlackboardComp->SetValueAsVector(TargetLocationKeyName, InPawn->GetActorLocation());
		}
	}
}

void ASZombieAIController::SetWayPoint(ASBotWayPoint * NewWayPoint)
{
	if (BlackboardComp) {
		BlackboardComp->SetValueAsObject(CurrentWayPointKeyName, NewWayPoint);
	}
}

void ASZombieAIController::SetTargetEnemy(APawn * NewTarget)
{
	if (BlackboardComp) {
		BlackboardComp->SetValueAsObject(TargetEnemyKeyName, NewTarget);
	}
}
