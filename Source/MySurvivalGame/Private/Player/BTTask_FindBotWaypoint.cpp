// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_FindBotWaypoint.h"
#include "SZombieAIController.h"
#include "SBotWayPoint.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/BlackboardComponent.h"




EBTNodeResult::Type UBTTask_FindBotWaypoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ASZombieAIController* MyController = Cast<ASZombieAIController>(OwnerComp.GetOwner());
	if (!MyController) {
		return EBTNodeResult::Failed;
	}

	ASBotWayPoint* CurrentWaypoint = MyController->GetWaypoint();
	AActor* NewWaypoint = nullptr;
	TArray<AActor*> AllWaypoint;
	UGameplayStatics::GetAllActorsOfClass(MyController, ASBotWayPoint::StaticClass(), AllWaypoint);
	if (AllWaypoint.Num())
		NewWaypoint = AllWaypoint[FMath::RandRange(0, AllWaypoint.Num() - 1)];
	if (NewWaypoint) {
		OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID(), NewWaypoint);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
