// Fill out your copyright notice in the Description page of Project Settings.

#include "SCharacter.h"
#include "SCharacterMovementComponent.h"
#include <GameFramework/PlayerInput.h>
#include <Engine/World.h>
#include <GameFramework/PlayerController.h>
#include <UnrealNetwork.h>
#include "SUsableActor.h"
#include <DrawDebugHelpers.h>
#include <GameFramework/DamageType.h>
#include <Class.h>
#include "SHUD.h"
#include <Components/SkeletalMeshComponent.h>
#include <Animation/AnimInstance.h>
#include <Components/CapsuleComponent.h>
#include <Engine/Engine.h>
#include <GameFramework/Actor.h>
#include <TimerManager.h>
#include "SWeapon.h"
#include "SWeaponPickUp.h"


ASCharacter::ASCharacter(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<USCharacterMovementComponent>(ACharacter::CharacterMovementComponentName)), SprintingSpeedScale(2.5f), 
	MaxUseDistance(500.0f), bHasNewFocus(false), FocusedUsableActor(nullptr), bIsTargeting(false), TargetingSpeedScale(0.5f),
	Health(100.0f), Hunger(0.0f), MaxHunger(100.0f), IncrementHungerAmount(1.0f), IncrementHungerInterval(5.0f), CriticalHungerThreshold(90.0f),HungerDamagePerInterval(1.0f),
	DropItemDistance(50.0f), 
	HungerDamageType(UDamageType::StaticClass())
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true;

	CameraSprintArm = CreateDefaultSubobject<USpringArmComponent>("CameraSprintArm");
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	CameraSprintArm->SocketOffset = FVector(0, 35, 0);
	CameraSprintArm->TargetOffset = FVector(0, 0, 55);
	CameraSprintArm->bUsePawnControlRotation = true;
	CameraSprintArm->SetupAttachment(GetRootComponent());
	Camera->SetupAttachment(CameraSprintArm);

	UCharacterMovementComponent *MoveComp = GetCharacterMovement();
	if (MoveComp) {
		MoveComp->GravityScale = 1.5f;
		MoveComp->JumpZVelocity = 620;
		MoveComp->bCanWalkOffLedgesWhenCrouching = true;
		MoveComp->MaxWalkSpeedCrouched = 200;

		MoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;
	}

	WeaponAttachPoint = TEXT("WeaponSocket");
	SpineAttachPoint = TEXT("SpineSocket");
	PelvisAttachPoint = TEXT("PelvisSocket");
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (Role == ROLE_Authority) {
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, this, &ASCharacter::IncrementHunger, IncrementHungerInterval, true);
	}
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Controller && Controller->IsLocalController()) {
		ASUsableActor *Usable = GetUsableView();

		if (FocusedUsableActor != Usable) {
			bHasNewFocus = true;
		}
		if (bHasNewFocus) {
			if (FocusedUsableActor && !Usable) FocusedUsableActor->OnEndFocus();
			else if (!FocusedUsableActor && Usable) Usable->OnStartFocus();

			bHasNewFocus = false;
		}
		FocusedUsableActor = Usable;
	}
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Lookup", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::OnStartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ASCharacter::OnStopJump);
	PlayerInputComponent->BindAction("CrouchToggle", IE_Released, this, &ASCharacter::OnCrouchToggle);
	PlayerInputComponent->BindAction("SprintHold", IE_Pressed, this, &ASCharacter::OnStartSprinting);
	PlayerInputComponent->BindAction("SprintHold", IE_Released, this, &ASCharacter::OnStopSprinting);
	PlayerInputComponent->BindAction("Use", IE_Released, this, &ASCharacter::Use);
	PlayerInputComponent->BindAction("Targeting", IE_Pressed, this, &ASCharacter::OnStartTargeting);
	PlayerInputComponent->BindAction("Targeting", IE_Released, this, &ASCharacter::OnStopTargeting);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASCharacter::OnStartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASCharacter::OnStopFire);
	PlayerInputComponent->BindAction("NextWeapon", IE_Released, this, &ASCharacter::OnNextWeapon);
	PlayerInputComponent->BindAction("PrevWeapon", IE_Released, this, &ASCharacter::OnPrevWeapon);
	PlayerInputComponent->BindAction("DropWeapon", IE_Released, this, &ASCharacter::DropWeapon);
	PlayerInputComponent->BindAction("EquipPrimaryWeapon", IE_Released, this, &ASCharacter::OnEquipPrimaryWeapon);
	PlayerInputComponent->BindAction("EquipSeconddaryWeapon", IE_Released, this, &ASCharacter::OnEquipSecondWeapon);
}

void ASCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode /* = 0 */)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	if (PrevMovementMode == MOVE_Falling && GetCharacterMovement()->MovementMode != MOVE_Falling) {
		SetIsJumping(false);
	}
}

float ASCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Health <= 0.0f) return 0.0f;
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage) {
		Health = FMath::Clamp(Health - ActualDamage, 0.0f, GetMaxHealth());

		if (Health) {
			//Play Hit
			APawn *Pawn = EventInstigator ? EventInstigator->GetPawn() : nullptr;
			PlayHit(Damage, DamageEvent, Pawn, DamageCauser, false);
		}
		else {
			//Death
			Die(Damage, DamageEvent, EventInstigator, DamageCauser);
		}
	}
	return ActualDamage;
}

bool ASCharacter::CanDie(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const
{
	if (bIsDying || IsPendingKill() || Role != ROLE_Authority || !GetWorld()->GetAuthGameMode())
		return false;
	else return true;
}

bool ASCharacter::Die(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!CanDie(Damage, DamageEvent, EventInstigator, DamageCauser))
		return false;
	else {
		const UDamageType *const DamageType = DamageEvent.DamageTypeClass ? DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>() : GetDefault<UDamageType>();
		//这个函数的作用在于保证EventInstigator不为nullptr;
		EventInstigator = GetDamageInstigator(EventInstigator, *DamageType);
		//这里在确定一下EventInstigator的情况，保证不会空指针
		OnDeath(Damage, DamageEvent, EventInstigator ? EventInstigator->GetPawn() : nullptr, DamageCauser);

		return true;
	}
}

void ASCharacter::OnDeath(float Damage, struct FDamageEvent const& DamageEvent, APawn* EventInstigator, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("OnDeath"));

	//bIsDying为true直接返回
	if (bIsDying) return;

	PlayHit(Damage, DamageEvent, EventInstigator, DamageCauser, true);

	//取消移动复制，停止Actor更新到新客服端
	bReplicateMovement = false;
	TearOff();
	bIsDying = true;

	//将Controller上分离Pawn，并通知销毁
	DetachFromControllerPendingDestroy();

	//停止所有的AnimMontages
	StopAllAnimMontages();

	//取消胶囊体的碰撞,忽略所有的通道
	UCapsuleComponent *CapsuleComp = GetCapsuleComponent();
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	//角色碰撞预设值设置为Ragdoll
	USkeletalMeshComponent *UseMesh = GetMesh();
	if (UseMesh) {
		UseMesh->SetCollisionProfileName(TEXT("Ragdoll"));
	}
	SetActorEnableCollision(true);
	SetRagdollPhysics();
}

void ASCharacter::PlayHit(float Damage, struct FDamageEvent const& DamageEvent, APawn* EventInstigator, AActor* DamageCauser, bool bKilled)
{
	if (Role == ROLE_Authority) ReplicateHit(Damage, DamageEvent, EventInstigator, DamageCauser, bKilled);

	if (Damage) ApplyDamageMomentum(Damage, DamageEvent, EventInstigator, DamageCauser);
}

void ASCharacter::MoveForward(float val)
{
	if (Controller && val) {
		const bool bLimitRotation = (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling());

		const FRotator Rotation = bLimitRotation ? GetActorRotation() : Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, val);
	}
}

void ASCharacter::MoveRight(float val)
{
	if (Controller && val) {
		const bool bLimitRotation = (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling());

		const FRotator Rotation = bLimitRotation ? GetActorRotation() : Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, val);
	}
}

void ASCharacter::OnStartJump()
{
	bPressedJump = true;

	SetIsJumping(true);
}

void ASCharacter::OnStopJump()
{
	bPressedJump = false;

}

bool ASCharacter::IsInitiatedJump() const
{
	return bIsJumping;
}

void ASCharacter::ServerSetJumping_Implementation(bool NewTargeting)
{
	SetIsJumping(NewTargeting);
}

bool ASCharacter::ServerSetJumping_Validate(bool NewTargeting)
{
	return true;
}

void ASCharacter::OnCrouchToggle()
{
	if (CanCrouch()) {
		Crouch();
	}
	else {
		UnCrouch();
	}
}

void ASCharacter::OnStartSprinting()
{
	SetIsSprinting(true);
}

void ASCharacter::OnStopSprinting()
{
	SetIsSprinting(false);
}

void ASCharacter::ServerSetSprinting_Implementation(bool NewTargeting)
{
	SetIsSprinting(NewTargeting);
}

bool ASCharacter::ServerSetSprinting_Validate(bool NewTargeting)
{
	return true;
}

bool ASCharacter::IsInitiatedSprinting() const
{
	if (!GetCharacterMovement()) return false;

	return bIsSprinting && !GetVelocity().IsZero() && FVector::DotProduct(GetVelocity().GetSafeNormal2D(), GetActorRotation().Vector()) > 0.8f;
}

float ASCharacter::GetSprintingSpeedScale() const
{
	return SprintingSpeedScale;
}

float ASCharacter::GetTargetingSpeedScale() const
{
	return TargetingSpeedScale;
}

void ASCharacter::ServerUse_Implementation()
{
	Use();
}

bool ASCharacter::ServerUse_Validate()
{
	return true;
}

bool ASCharacter::IsTargeting() const
{
	return bIsTargeting;
}

void ASCharacter::ServerSetTargeting_Implementation(bool NewTargeting)
{
	SetTargeting(NewTargeting);
}

bool ASCharacter::ServerSetTargeting_Validate(bool NewTargeting)
{
	return true;
}

FRotator ASCharacter::GetAimOffsets() const
{
	const FVector AimDirWs = GetBaseAimRotation().Vector();
	const FVector AimDirLs = ActorToWorld().InverseTransformVectorNoScale(AimDirWs);
	const FRotator AimRotLS = AimDirLs.Rotation();

	return AimRotLS;
}

float ASCharacter::GetHealth() const
{
	return Health;
}

float ASCharacter::GetMaxHealth() const
{
	return GetClass()->GetDefaultObject<ASCharacter>()->GetHealth();
}

float ASCharacter::GetHunger() const
{
	return Hunger;
}

float ASCharacter::GetMaxHunger() const
{
	return MaxHunger;
}

void ASCharacter::ConsumeFood(float AmountRestored)
{
	Hunger = FMath::Clamp(Hunger - AmountRestored, 0.0f, GetMaxHunger());
	Health = FMath::Clamp(Health + AmountRestored, 0.0f, GetMaxHealth());

	APlayerController *PC = Cast<APlayerController>(Controller);
	if (PC) {
		ASHUD *MyHud = Cast<ASHUD>(PC->GetHUD());
		if (MyHud) {
			MyHud->MessageReceived("Food item consumed!");
		}
	}
}

bool ASCharacter::IsAlive() const
{
	return GetHealth() > 0.0f;
}

void ASCharacter::ReplicateHit(float Damage, struct FDamageEvent const& DamageEvent, APawn* EventInstigator, AActor* DamageCauser, bool bKilled)
{
	FDamageEvent const& LastDamageEvent = LastTakeHitInfo.GetDamageEvent();
	if (EventInstigator == LastTakeHitInfo.PawnInstigator.Get() && LastDamageEvent.DamageTypeClass == DamageEvent.DamageTypeClass) {
		if (bKilled && LastTakeHitInfo.bKilled) {
			return;
		}

		Damage += LastTakeHitInfo.ActualDamage;
	}

	LastTakeHitInfo.ActualDamage = Damage;
	LastTakeHitInfo.PawnInstigator = Cast<ASCharacter>(EventInstigator);
	LastTakeHitInfo.SetDamageEvent(DamageEvent);
	LastTakeHitInfo.DamageCauser = DamageCauser;
	LastTakeHitInfo.bKilled = bKilled;
	LastTakeHitInfo.EnsureReplication();
}

void ASCharacter::OnRep_LastTakeHitInfo()
{
	if (LastTakeHitInfo.bKilled) {
		OnDeath(LastTakeHitInfo.ActualDamage, LastTakeHitInfo.GetDamageEvent(), LastTakeHitInfo.PawnInstigator.Get(), LastTakeHitInfo.DamageCauser.Get());
	}
	else {
		PlayHit(LastTakeHitInfo.ActualDamage, LastTakeHitInfo.GetDamageEvent(), LastTakeHitInfo.PawnInstigator.Get(), LastTakeHitInfo.DamageCauser.Get(), LastTakeHitInfo.bKilled);
	}
}

void ASCharacter::SetIsJumping(bool NewJumping)
{
	if (bIsCrouched && NewJumping) {
		UnCrouch();
	}else 
		bIsJumping = NewJumping;

	if (Role < ROLE_Authority) {
		ServerSetJumping(NewJumping);
	}
}

void ASCharacter::SetIsSprinting(bool NewSprinting)
{
	bIsSprinting = NewSprinting;
	if (bIsCrouched && NewSprinting) {
		UnCrouch();
	}

	if (Role < ROLE_Authority) {
		ServerSetSprinting(NewSprinting);
	}
}

ASUsableActor * ASCharacter::GetUsableView()
{
	FVector CameraLoc;
	FRotator CameraRot;

	if (!GetController())
		return nullptr;

	GetController()->GetPlayerViewPoint(CameraLoc, CameraRot);

	const FVector TraceStart = CameraLoc;
	const FVector Direction = CameraRot.Vector();
	const FVector TraceEnd = TraceStart + Direction * MaxUseDistance;

	FHitResult Hit(ForceInit);
	FCollisionQueryParams TraceParams;
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;
	TraceParams.bTraceComplex = true;

	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, TraceParams);
	//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 1.0f);

	return Cast<ASUsableActor>(Hit.GetActor());
}

void ASCharacter::Use()
{
	ASUsableActor *Usable = GetUsableView();
	if (Usable) {
		Usable->OnUsed(this);
	}

	if (Role < ROLE_Authority) ServerUse();
}

void ASCharacter::OnStartTargeting()
{
	SetTargeting(true);
}

void ASCharacter::OnStopTargeting()
{
	SetTargeting(false);
}

void ASCharacter::SetTargeting(bool NewTargeting)
{
	bIsTargeting = NewTargeting;

	if (Role < ROLE_Authority) ServerSetTargeting(NewTargeting);
}

void ASCharacter::IncrementHunger()
{
	Hunger = FMath::Clamp(Hunger + IncrementHungerAmount, 0.0f, GetMaxHunger());

	if (Hunger > CriticalHungerThreshold) {
		FDamageEvent DmgEvent;
		DmgEvent.DamageTypeClass = HungerDamageType;
		
		TakeDamage(HungerDamagePerInterval, DmgEvent, GetController(), this);
	}
}

void ASCharacter::SetRagdollPhysics()
{
	UE_LOG(LogTemp, Warning, TEXT("SetRagdollPhysics"));

	bool bInRagdoll = false;
	USkeletalMeshComponent *UseMesh = GetMesh();
	if (!IsPendingKill() && UseMesh && UseMesh->GetPhysicsAsset()) {
		bInRagdoll = true;
		UseMesh->SetAllBodiesSimulatePhysics(true);
		UseMesh->SetSimulatePhysics(true);
		UseMesh->WakeAllRigidBodies();
		UseMesh->bBlendPhysics = true;
	}

	UCharacterMovementComponent *CharacterComp = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (CharacterComp) {
		CharacterComp->StopMovementImmediately();
		CharacterComp->DisableMovement();
		CharacterComp->SetComponentTickEnabled(false);
	}

	if (!bInRagdoll) {
		TurnOff();
		SetActorHiddenInGame(true);
		SetLifeSpan(1.0f);
	}
	else SetLifeSpan(10.0f);
}

void ASCharacter::StopAllAnimMontages()
{
	USkeletalMeshComponent *UseMesh = GetMesh();
	if (UseMesh && UseMesh->AnimScriptInstance) {
		UseMesh->AnimScriptInstance->Montage_Stop(0.0f);
	}
}

void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty, FDefaultAllocator> &OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//DOREPLIFETIME(ASCharacter, bIsJumping);
	DOREPLIFETIME_CONDITION(ASCharacter, bIsJumping, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ASCharacter, bIsSprinting, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ASCharacter, bIsTargeting, COND_SkipOwner);
	DOREPLIFETIME(ASCharacter, Health);
	DOREPLIFETIME(ASCharacter, Hunger);
	DOREPLIFETIME(ASCharacter, LastTakeHitInfo);
	DOREPLIFETIME(ASCharacter, CurrentWeapon);
	DOREPLIFETIME(ASCharacter, Inventory);
}

void ASCharacter::OnStartFire()
{
	if (IsInitiatedSprinting()) SetIsSprinting(false);

	StartWeaponFire();
}

void ASCharacter::OnStopFire()
{
	StopWeaponFire();
}

void ASCharacter::OnNextWeapon()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "OnNextWeapon");

	if (Inventory.Num() > 1) {
		int index = Inventory.IndexOfByKey(CurrentWeapon);
		int nextIndex = (index + 1) % Inventory.Num();
		EquipWeapon(Inventory[nextIndex]);
	}
}

void ASCharacter::OnPrevWeapon()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "OnPrevWeapon");

	if (Inventory.Num() > 1) {
		int index = Inventory.IndexOfByKey(CurrentWeapon);
		int prevIndex = FMath::Abs(index - 1) % Inventory.Num();
		EquipWeapon(Inventory[prevIndex]);
	}
}

void ASCharacter::OnEquipPrimaryWeapon()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "EquipPrimaryWeapon");

	if (CurrentWeapon->GetStorageSlot() != EInventorySlot::Primary) {
		for (ASWeapon *item : Inventory) {
			if (item->GetStorageSlot() == EInventorySlot::Primary) {
				EquipWeapon(item);
				break;
			}
		}
	}

}

void ASCharacter::OnEquipSecondWeapon()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "EquipSecondWeapon");

	if (CurrentWeapon->GetStorageSlot() != EInventorySlot::Secondary) {
		for (ASWeapon *item : Inventory) {
			if (item->GetStorageSlot() == EInventorySlot::Secondary) {
				EquipWeapon(item);
				break;
			}
		}
	}
}

void ASCharacter::StartWeaponFire()
{
	if (!bWantsToFire) {
		bWantsToFire = true;
		if (CurrentWeapon) CurrentWeapon->StartFire();
	}
}

void ASCharacter::StopWeaponFire()
{
	if (bWantsToFire) {
		bWantsToFire = false;
		if (CurrentWeapon) CurrentWeapon->StopFire();
	}
}

void ASCharacter::DestroyInventory()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "DestroyInventory");

}

void ASCharacter::DropWeapon()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "DropWeapon");

	if (Role < ROLE_Authority) {
		ServerDropWeapon();
		return;
	}

	if (CurrentWeapon) {
		FVector CameraLoc;
		FRotator CameraRot;
		if (!Controller) return;
		Controller->GetPlayerViewPoint(CameraLoc, CameraRot);
		const FVector Direction = CameraRot.Vector();
		const FVector SpawnLocation = GetActorLocation() + (Direction * DropItemDistance);

		if (CurrentWeapon->WeaponPickupClass) {
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			ASWeaponPickUp* NewWeaponPickup = GetWorld()->SpawnActor<ASWeaponPickUp>(CurrentWeapon->WeaponPickupClass, SpawnLocation, FRotator::ZeroRotator, SpawnInfo);
			UStaticMeshComponent *MeshComp = NewWeaponPickup->GetUsableMeshComponent();
			if (MeshComp) {
				MeshComp->AddTorqueInRadians(FVector(1, 1, 1) * 40000);
			}
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "WeaponPickupClass is null");
		}
	}

	RemoveWeapon(CurrentWeapon);
}

void ASCharacter::OnRep_CurrentWeapon(ASWeapon *LastWeapon)
{
	SetCurrentWeapon(CurrentWeapon, LastWeapon);
}

bool ASCharacter::WeaponSlotAvailable(EInventorySlot CheckSlot)
{
	TArray<ASWeapon*> SubArray = Inventory.FilterByPredicate([CheckSlot](const ASWeapon *Weapon) -> bool {if (CheckSlot == Weapon->GetStorageSlot()) return true; else return false; });
	if (SubArray.Num()) return false;
	else return true;
}

bool ASCharacter::CanFire() const
{
	if (CurrentWeapon && IsAlive())
		return true;
	else return false;
}

bool ASCharacter::IsFiring() const
{
	return CurrentWeapon && CurrentWeapon->GetCurrentState() == EWeaponState::Firing;
}

FName ASCharacter::GetInventoryAttachPoint(EInventorySlot Slot)
{
	FName ret;
	switch (Slot)
	{
	case EInventorySlot::Hands:
		ret = WeaponAttachPoint;
		break;
	case EInventorySlot::Primary:
		ret = SpineAttachPoint;
		break;
	case EInventorySlot::Secondary:
		ret = PelvisAttachPoint;
		break;
	default:
		break;
	}
	return ret;
}

void ASCharacter::SetCurrentWeapon(ASWeapon *NewWeapon, ASWeapon *LastWeapon /*= nullptr*/)
{
	ASWeapon *LocalLastWeapon = nullptr;

	if (LastWeapon) {
		LocalLastWeapon = LastWeapon;
	}
	else if (NewWeapon != CurrentWeapon) {
		LocalLastWeapon = CurrentWeapon;
	}

	if (LocalLastWeapon) {
		//先卸载之前的武器
		LocalLastWeapon->OnUnEquip();
	}

	CurrentWeapon = NewWeapon;
	if (NewWeapon) {
		NewWeapon->SetOwningPawn(this);
		NewWeapon->OnEquip();
	}
}

void ASCharacter::EquipWeapon(ASWeapon *NewWeapon)
{
	if (NewWeapon) {
		if (Role == ROLE_Authority) {
			SetCurrentWeapon(NewWeapon);
		}
		else {
			ServerEquipWeapon(NewWeapon);
		}
	}
}

void ASCharacter::AddWeapon(ASWeapon *Weapon)
{
	if (Weapon && Role == ROLE_Authority) {
		Weapon->OnEnterInventory(this);
		Inventory.AddUnique(Weapon);

		if (!CurrentWeapon && Inventory.Num()) {
			EquipWeapon(Inventory[0]);
		}
	}
}

void ASCharacter::RemoveWeapon(ASWeapon *Weapon)
{
	if (Weapon) {
		if (Inventory.Contains(Weapon))
			Inventory.Remove(Weapon);
		Weapon->Mesh->SetHiddenInGame(true);
		Weapon->Destroy();

		SetCurrentWeapon(nullptr);
		if (Inventory.Num()) EquipWeapon(Inventory[0]);
	}
}

void ASCharacter::ServerEquipWeapon_Implementation(ASWeapon *NewWeapon)
{
	EquipWeapon(NewWeapon);
}

bool ASCharacter::ServerEquipWeapon_Validate(ASWeapon *NewWeapon)
{
	return true;
}

void ASCharacter::ServerDropWeapon_Implementation()
{
	DropWeapon();
}

bool ASCharacter::ServerDropWeapon_Validate()
{
	return true;
}
