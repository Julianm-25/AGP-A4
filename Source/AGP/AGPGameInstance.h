// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "Characters/EnemyCharacter.h"
#include "Engine/GameInstance.h"
#include "AGPGameInstance.generated.h"

class AWeaponPickup;
/**
 * 
 */
UCLASS()
class AGP_API UAGPGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UClass* GetWeaponPickupClass() const;
	UClass* GetEnemyCharacterClass() const;
	void SpawnMuzzleFlashParticles(const FVector& SpawnLocation, const FRotator& SpawnRotation);
	void SpawnCharacterHitParticles(const FVector& SpawnLocation);
	void SpawnTerrainHitParticles(const FVector& SpawnLocation);
protected:

	UPROPERTY(EditDefaultsOnly, Category="Pickup Classes")
	TSubclassOf<AWeaponPickup> WeaponPickupClass;
	UPROPERTY(EditDefaultsOnly, Category="Enemy Classes")
	TSubclassOf<AEnemyCharacter> EnemyCharacterClass;
	UPROPERTY(EditDefaultsOnly, Category="Particle Systems")
	UNiagaraSystem* MuzzleFlashParticles;
	UPROPERTY(EditDefaultsOnly, Category="Particle Systems")
	UNiagaraSystem* CharacterHitParticles;
	UPROPERTY(EditDefaultsOnly, Category="Particle Systems")
	UNiagaraSystem* TerrainHitParticles;
};
