// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Stypes.h"
#include "SWeapon.generated.h"

UENUM()
enum EWeaponState
{
	Idle,
	Firing,
	Equipping,
	Reloading
};

UCLASS()
class MYSURVIVALGAME_API ASWeapon : public AActor
{
	GENERATED_UCLASS_BODY()
	
public:	
	// Sets default values for this actor's properties
	//ASWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleDefaultsOnly, Category = Components)
		USkeletalMeshComponent *Mesh;

public:
	EWeaponState GetCurrentState() const;
	FORCEINLINE EInventorySlot GetStorageSlot() const
	{
		return StorageSlot;
	}

	//���뱳���󸽼ӵ����ص�
	virtual void OnEnterInventory(ASCharacter *NewOwner);

	//����Owner
	void SetOwningPawn(ASCharacter *NewOwner);

	//�������ӵ���ɫ���ص㣬Ĭ�Ϲ���������
	void AttachMeshToPawn(EInventorySlot Slot = EInventorySlot::Hands);

	//������ж������
	void DetachMeshFromPawn();

public:
	UFUNCTION()
		void OnRep_OwnerPawn();

	UPROPERTY(Transient, ReplicatedUsing = OnRep_OwnerPawn)
		ASCharacter *OwnerPawn;

private:
	EWeaponState CurrentState;
	EInventorySlot StorageSlot;
};
