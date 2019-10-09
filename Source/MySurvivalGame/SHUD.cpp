// Fill out your copyright notice in the Description page of Project Settings.

#include "SHUD.h"
#include <ConstructorHelpers.h>
#include "SPlayerController.h"
#include "SCharacter.h"


ASHUD::ASHUD(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer)
{
	ConstructorHelpers::FObjectFinder<UTexture2D> HUDCenterDotObj(TEXT("Texture2D'/Game/UI/HUD/T_CenterDot_M.T_CenterDot_M'"));
	if (HUDCenterDotObj.Succeeded()) {
		CenterDotIcon = UCanvas::MakeIcon(HUDCenterDotObj.Object);
	}
}

void ASHUD::DrawHUD()
{
	Super::DrawHUD();

	DrawDot();
}

void ASHUD::DrawDot()
{
	float CenterX = Canvas->ClipX / 2;
	float CenterY = Canvas->ClipY / 2;

	float CenterDotScale = 0.07f;

	ASPlayerController *PCOwner = Cast<ASPlayerController>(PlayerOwner);
	if (PCOwner) {
		ASCharacter *Pawn = Cast<ASCharacter>(PCOwner->GetPawn());

		if (Pawn) { //TODO
			Canvas->SetDrawColor(255, 255, 255, 255);
			Canvas->DrawIcon(CenterDotIcon,
				CenterX - CenterDotIcon.UL * CenterDotScale / 2.0f,
				CenterY - CenterDotIcon.VL * CenterDotScale / 2.0f,
				CenterDotScale);
		}
	}
}
