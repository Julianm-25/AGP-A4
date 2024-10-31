// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "Characters/AlliedCharacter.h"
#include "Characters/EnemyCharacter.h"
#include "Engine/GameInstance.h"
#include "Sound/SoundCue.h"
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
	UClass* GetAlliedCharacterClass() const;
	void SpawnMuzzleFlashParticles(const FVector& SpawnLocation, const FRotator& SpawnRotation);
	void SpawnCharacterHitParticles(const FVector& SpawnLocation);
	void SpawnTerrainHitParticles(const FVector& SpawnLocation);
	void SpawnBulletParticles(const FVector& SpawnLocation, const FRotator& SpawnRotation);
	void PlayGunshotSoundAtLocation(const FVector& Location);
	void PlayGunshotSound2D();
	void PlayMeleeHitSoundAtLocation(const FVector& Location);
protected:

	UPROPERTY(EditDefaultsOnly, Category="Pickup Classes")
	TSubclassOf<AWeaponPickup> WeaponPickupClass;
	UPROPERTY(EditDefaultsOnly, Category="Enemy Classes")
	TSubclassOf<AEnemyCharacter> EnemyCharacterClass;
	UPROPERTY(EditDefaultsOnly, Category="Allied Classes")
	TSubclassOf<AAlliedCharacter> AlliedCharacterClass;
	UPROPERTY(EditDefaultsOnly, Category="Particle Systems")
	UNiagaraSystem* MuzzleFlashParticles;
	UPROPERTY(EditDefaultsOnly, Category="Particle Systems")
	UNiagaraSystem* CharacterHitParticles;
	UPROPERTY(EditDefaultsOnly, Category="Particle Systems")
	UNiagaraSystem* TerrainHitParticles;
	UPROPERTY(EditDefaultsOnly, Category="Particle Systems")
	UNiagaraSystem* BulletParticles;
	UPROPERTY(EditDefaultsOnly, Category="Sound Cues")
	USoundCue* GunshotSoundCue;
	UPROPERTY(EditDefaultsOnly, Category="Sound Cues")
	USoundCue* MeleeHitSoundCue;
};
