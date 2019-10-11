// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SUsableActor.generated.h"

UCLASS()
class MYSURVIVALGAME_API ASUsableActor : public AActor
{
	GENERATED_UCLASS_BODY()
	
public:	
	// Sets default values for this actor's properties

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = Interface)
		virtual void OnStartFocus();
	UFUNCTION(BlueprintCallable, Category = Interface)
		virtual void OnEndFocus();
	UFUNCTION(BlueprintCallable, Category = Interface)
		virtual void OnUsed(APawn *Pawn);

protected:
	UPROPERTY(VisibleAnywhere, Category = Mesh)
		UStaticMeshComponent *MeshComp;
};
