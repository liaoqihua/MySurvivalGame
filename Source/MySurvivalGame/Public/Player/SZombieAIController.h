// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SZombieAIController.generated.h"

class ASBotWayPoint;
class ASBaseCharacter;
class UBehaviorTreeComponent;

/**
 * 
 */
UCLASS()
class MYSURVIVALGAME_API ASZombieAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ASZombieAIController(const FObjectInitializer &ObjectInitializer);

	virtual void Possess(APawn* InPawn) override;

public:

	UBehaviorTreeComponent *BehaviorComp;

	UBlackboardComponent *BlackboardComp;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
		FName TargetEnemyKeyName;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
		FName TargetLocationKeyName;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI")
		FName PatrolLocationKeyName;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
		FName CurrentWayPointKeyName;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
		FName BotTypeKeyName;

public:
	ASBotWayPoint *GetWaypoint();

	ASBaseCharacter *GetTargetEnemy();

	void SetMoveToTarget(APawn *InPawn);

	void SetWayPoint(ASBotWayPoint *NewWayPoint);

	void SetTargetEnemy(APawn *NewTarget);

	FORCEINLINE UBehaviorTreeComponent* GetBehaviorComp() const {
		return BehaviorComp;
	}

	FORCEINLINE UBlackboardComponent* GetBlackboardComp() const {
		return BlackboardComp;
	}
};
