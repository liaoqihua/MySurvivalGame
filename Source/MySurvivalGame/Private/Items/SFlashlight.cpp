// Fill out your copyright notice in the Description page of Project Settings.

#include "SFlashlight.h"
#include "UnrealNetwork.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetStringLibrary.h"


ASFlashlight::ASFlashlight(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer), EmissiveParamName("Brightness"), MaxEmissiveIntensity(20.0f), LastEmissiveStrength(-1.0f), bIsActive(true)
{
	StorageSlot = EInventorySlot::Secondary;
	LightAttachPoint = "LightSocket";
	GetWeaponMesh()->AddLocalRotation(FRotator(0, 0, -90));

	LightConeComp = CreateDefaultSubobject<UStaticMeshComponent>("LightConeComp");
	LightConeComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	LightConeComp->SetupAttachment(GetWeaponMesh(), LightAttachPoint);
	LightConeComp->SetRelativeRotation(FRotator(0, 0, -90));

	SpotLightComp = CreateDefaultSubobject<USpotLightComponent>("SpotLightComp");
	SpotLightComp->SetupAttachment(GetWeaponMesh(), LightAttachPoint);
	SpotLightComp->SetCastShadows(false);
	SpotLightComp->SetRelativeRotation(FRotator(0, 0, 0));
}

void ASFlashlight::BeginPlay()
{
	Super::BeginPlay();
	UpdateLight(false);
}

void ASFlashlight::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (GetWeaponMesh()->SkeletalMesh)
		MID = GetWeaponMesh()->CreateAndSetMaterialInstanceDynamic(0);
}

void ASFlashlight::OnUnEquip()
{
	Super::OnUnEquip();

	UpdateLight(false);
}

void ASFlashlight::OnEquipFinished()
{
	Super::OnEquipFinished();

	UpdateLight(true);
}

void ASFlashlight::OnEquip()
{
	Super::OnEquip();

	UpdateLight(false);
}

void ASFlashlight::OnEnterInventory(ASCharacter * NewOwner)
{
	Super::OnEnterInventory(NewOwner);
}

void ASFlashlight::OnLeaveInventory()
{
	Super::OnLeaveInventory();
}

void ASFlashlight::UpdateLight(bool Enabled)
{
	if (MID) {
		MID->SetScalarParameterValue(EmissiveParamName, Enabled ? MaxEmissiveIntensity : 0.0f);
		SpotLightComp->SetVisibility(Enabled);
		LightConeComp->SetVisibility(Enabled);
	}

	bIsActive = Enabled;
}

void ASFlashlight::OnRep_IsActive(bool prevActive)
{
	if (prevActive != bIsActive && Role < ROLE_Authority)
		UpdateLight(bIsActive);
}

void ASFlashlight::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASFlashlight, bIsActive);
}