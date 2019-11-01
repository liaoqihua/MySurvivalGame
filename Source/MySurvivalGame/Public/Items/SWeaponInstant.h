// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/SWeapon.h"
#include "SWeaponInstant.generated.h"

USTRUCT()
struct FHitOriginNotify
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		FVector OriginPoint;

private:
	UPROPERTY()
		int8 EnsureReplicatedByte;

public:
	FHitOriginNotify() :EnsureReplicatedByte(0) {

	}

	void EnsureReplication() {
		EnsureReplicatedByte++;
	}
};

/**
 * 
 */
UCLASS()
class MYSURVIVALGAME_API ASWeaponInstant : public ASWeapon
{
	GENERATED_UCLASS_BODY()
	
protected:
	virtual void FireWeapon() override;
	
	UPROPERTY(EditDefaultsOnly)
		float WeaponRange;

public:
	//��ֵ
	UPROPERTY(EditDefaultsOnly)
		float AllowedViewDotHitDir;

	//���еķ�Χ�ͱ���
	UPROPERTY(EditDefaultsOnly)
		float ClientSideHitLeeway;

	void ProcessInstantHit(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir);

	//֪ͨ����������
	UFUNCTION(Reliable, Server, WithValidation)
		void ServerNotifyHit(const FHitResult Impact, FVector_NetQuantizeNormal ShootDir);

	//ȷ������
	void ProcessInstantHitConfirmed(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir);

	//֪ͨ������ û����
	UFUNCTION(Reliable, Server, WithValidation)
		void ServerNotifyMiss(FVector_NetQuantizeNormal ShootDir);

	void DealDamage(const FHitResult &Impact, const FVector &ShootDir);

	bool ShouldDealDamage(AActor *TestActor) const;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_HitOriginNotify)
		FHitOriginNotify HitOriginNotify;

	UFUNCTION()
		void OnRep_HitOriginNotify();

	UPROPERTY(EditDefaultsOnly)
		float HitDamage;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UDamageType> DamageTypeClass;

public:
	//ģ�� ���������Ч��
	void SimulateInstantHit(const FVector &Origin);

	//���к����ѪЧ��
	void SpawnImpactEffects(const FHitResult &Impact);

	//���Ч�� �ӵ���Ч������
	void SpawnTrailEffects(const FVector& EndPoint);

public:
	UPROPERTY(EditDefaultsOnly)
		UParticleSystem *TrailFX;

	UPROPERTY(EditDefaultsOnly)
		UParticleSystem *TracerFX;

	//UPROPERTY()
	//	class UParticleSystemComponent *TrailPSC;

	UPROPERTY(EditDefaultsOnly)
		FName TrailTargetParam;

	UPROPERTY(EditDefaultsOnly)
		float MinimumProjectileSpawnDistance;

	UPROPERTY(EditDefaultsOnly)
		int32 TracerRoundInterval;

	int32 BulletsShotCount;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class ASImpactEffect> ImpactTemplate;
};
