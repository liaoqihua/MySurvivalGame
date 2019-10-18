// Fill out your copyright notice in the Description page of Project Settings.

#include "SUsableActor.h"
#include <Components/StaticMeshComponent.h>


// Sets default values
ASUsableActor::ASUsableActor(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, "MeshComp");
	RootComponent = (USceneComponent*)MeshComp;
}

// Called when the game starts or when spawned
void ASUsableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASUsableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASUsableActor::OnStartFocus()
{
	MeshComp->SetRenderCustomDepth(true);
}

void ASUsableActor::OnEndFocus()
{
	MeshComp->SetRenderCustomDepth(false);
}

void ASUsableActor::OnUsed(APawn *InstigatorPawn)
{
	UE_LOG(LogTemp, Warning, TEXT("OnUsed"));
}

