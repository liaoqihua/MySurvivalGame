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
	//阈值
	UPROPERTY(EditDefaultsOnly)
		float AllowedViewDotHitDir;

	//命中的范围和比例
	UPROPERTY(EditDefaultsOnly)
		float ClientSideHitLeeway;

	void ProcessInstantHit(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir);

	//通知服务器命中
	UFUNCTION(Reliable, Server, WithValidation)
		void ServerNotifyHit(const FHitResult Impact, FVector_NetQuantizeNormal ShootDir);

	//确认命中
	void ProcessInstantHitConfirmed(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir);

	//通知服务器 没打着
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
	//模拟 击中与射击效果
	void SimulateInstantHit(const FVector &Origin);

	//命中后的流血效果
	void SpawnImpactEffects(const FHitResult &Impact);

	//射击效果 子弹的效果生成
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
