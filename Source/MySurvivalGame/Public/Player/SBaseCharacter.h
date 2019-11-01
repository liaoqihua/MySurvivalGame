// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Stypes.h"
#include "SBaseCharacter.generated.h"

UCLASS()
class MYSURVIVALGAME_API ASBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASBaseCharacter(const FObjectInitializer &ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	UFUNCTION(BlueprintCallable, Category = PlayerCondition)
		float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = PlayerCondition)
		float GetMaxHealth() const;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void PlayHit(float Damage, struct FDamageEvent const& DamageEvent, APawn* EventInstigator, AActor* DamageCauser, bool bKilled);
	virtual bool Die(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);
	virtual bool CanDie(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const;
	virtual void OnDeath(float Damage, struct FDamageEvent const& DamageEvent, APawn* EventInstigator, AActor* DamageCauser);


	UFUNCTION()
		virtual void ReplicateHit(float Damage, struct FDamageEvent const& DamageEvent, APawn* EventInstigator, AActor* DamageCauser, bool bKilled);

	void SetRagdollPhysics();

	UFUNCTION(BlueprintCallable, Category = PlayerCondition)
		bool IsAlive() const;

public:
	//µ±Ç°½¡¿µ
	UPROPERTY(EditDefaultsOnly, Category = PlayerCondition, Replicated)
		float Health;

	bool bIsDying;

	UPROPERTY(EditDefaultsOnly)
		UPawnNoiseEmitterComponent *NoiseEmitterComp;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_LastTakeHitInfo)
		struct FTakeHitInfo LastTakeHitInfo;

	UFUNCTION()
		void OnRep_LastTakeHitInfo();
};
