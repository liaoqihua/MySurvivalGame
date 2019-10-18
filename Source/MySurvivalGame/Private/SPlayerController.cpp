// Fill out your copyright notice in the Description page of Project Settings.

#include "SPlayerController.h"
#include "Player/SPlayerCameraManager.h"

ASPlayerController::ASPlayerController(const FObjectInitializer &oi)
	:Super(oi)
{
	PlayerCameraManagerClass = ASPlayerCameraManager::StaticClass();
}


