// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/SBaseCharacter.h"
#include "Stypes.h"
#include "SZombieCharacter.generated.h"

/**
 * 
 */
UCLASS()
class MYSURVIVALGAME_API ASZombieCharacter : public ASBaseCharacter
{
	GENERATED_BODY()
	
public:
	ASZombieCharacter(const FObjectInitializer &ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION()
		void OnSeePlayer(APawn* Pawn);

	UFUNCTION()
		void OnHearNoise(APawn* MyInstigator, const FVector& Location, float Volume);

public:
	float LastSeenTime;
	
	float LastHeardTime;

	float PunchDamage;

	bool bSenseTarget;

	UPROPERTY(EditAnywhere, Category = "AI")
		EBotBehaviorType BotType;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
		class UBehaviorTree* BehaviorTree;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
		float SenseTimeOut;
	
	UPROPERTY(VisibleAnywhere, Category = "AI")
		class UPawnSensingComponent *PawnSensingComp;
};
