// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_FindPatrolLocation.h"
#include "SZombieAIController.h"
#include "Kismet/GameplayStatics.h"
#include "SBotWayPoint.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"




EBTNodeResult::Type UBTTask_FindPatrolLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ASZombieAIController* MyController = Cast<ASZombieAIController>(OwnerComp.GetOwner());
	if (!MyController) {
		return EBTNodeResult::Failed;
	}

	ASBotWayPoint* CurrentWaypoint = MyController->GetWaypoint();
	if (CurrentWaypoint) {
		const float SearchRadius = 200.0f;
		const FVector SearchOrigin = CurrentWaypoint->GetActorLocation();

		FVector Loc = FVector::ZeroVector;
		UNavigationSystemV1::K2_GetRandomPointInNavigableRadius(GetWorld(), SearchOrigin, Loc, SearchRadius);

		if (Loc != FVector::ZeroVector) {
			OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID(), Loc);
			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;

}
