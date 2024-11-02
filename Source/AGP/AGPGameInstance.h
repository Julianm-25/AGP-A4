// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
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

	
	UClass* GetWeaponPickupClass() const; // Getter for the weapon pickup
	UClass* GetEnemyCharacterClass() const; // Getter for the enemy characters
	void SpawnMuzzleFlashParticles(const FVector& SpawnLocation, const FRotator& SpawnRotation) const; // Spawns the muzzle flash particle system at the given location with the given rotation
	void SpawnCharacterHitParticles(const FVector& SpawnLocation) const; // Spawns the character hit particle system at the given location
	void SpawnTerrainHitParticles(const FVector& SpawnLocation) const; // Spawns the terrain hit particle system at the given location
	void SpawnBulletParticles(const FVector& SpawnLocation, const FRotator& SpawnRotation) const; // Spawns the bullet particle system at the given location with the given rotation
	void PlayGunshotSoundAtLocation(const FVector& Location) const; // Plays the gunshot sound cue at the given location
	void PlayGunshotSound2D() const; // Plays the gunshot sound without a direction, used for the character that fires the gun
	void PlayMeleeHitSoundAtLocation(const FVector& Location) const; // Plays the melee hit sound at the given location
protected:

	UPROPERTY(EditDefaultsOnly, Category="Pickup Classes")
	TSubclassOf<AWeaponPickup> WeaponPickupClass; // Reference to the gun pickup, used for spawning guns in
	UPROPERTY(EditDefaultsOnly, Category="Enemy Classes")
	TSubclassOf<AEnemyCharacter> EnemyCharacterClass; // Reference to the enemy character, used for spawning enemies in
	UPROPERTY(EditDefaultsOnly, Category="Particle Systems")
	UNiagaraSystem* MuzzleFlashParticles; // Particle system for the flash when a gun is fired
	UPROPERTY(EditDefaultsOnly, Category="Particle Systems")
	UNiagaraSystem* CharacterHitParticles; // Particle system for when a shot hits a character
	UPROPERTY(EditDefaultsOnly, Category="Particle Systems")
	UNiagaraSystem* TerrainHitParticles; // Particle system for when a shot hits terrain
	UPROPERTY(EditDefaultsOnly, Category="Particle Systems")
	UNiagaraSystem* BulletParticles; // Particle system for the shot coming out of a player's gun
	UPROPERTY(EditDefaultsOnly, Category="Sound Cues")
	USoundCue* GunshotSoundCue; // Sound for when a player fires a gun
	UPROPERTY(EditDefaultsOnly, Category="Sound Cues")
	USoundCue* MeleeHitSoundCue; // Sound for when an enemy hits a player
};
