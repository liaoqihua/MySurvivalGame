// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include <Engine/Canvas.h>
#include "SHUD.generated.h"

/**
 * 
 */
UCLASS()
class MYSURVIVALGAME_API ASHUD : public AHUD
{
	GENERATED_UCLASS_BODY()

public:
	virtual void DrawHUD() override;

public:
	void DrawDot();

public:
	UFUNCTION(BlueprintImplementableEvent, Category = HUDEvents)
		void MessageReceived(const FString &TextMessage);
	
public:
	FCanvasIcon CenterDotIcon;
};
