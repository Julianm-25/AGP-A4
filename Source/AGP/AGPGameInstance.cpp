 // Fill out your copyright notice in the Description page of Project Settings.
#include "AGPGameInstance.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Pickups/WeaponPickup.h"

UClass* UAGPGameInstance::GetWeaponPickupClass() const // Getter for the weapon pickup
{
 return WeaponPickupClass.Get();
}

 UClass* UAGPGameInstance::GetEnemyCharacterClass() const // Getter for the enemy characters
 {
 return EnemyCharacterClass.Get();
 }

 void UAGPGameInstance::SpawnMuzzleFlashParticles(const FVector& SpawnLocation, const FRotator& SpawnRotation) const  // Spawns the muzzle flash particle system at the given location with the given rotation
 {
 UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), MuzzleFlashParticles, SpawnLocation, SpawnRotation);
 }

 void UAGPGameInstance::SpawnCharacterHitParticles(const FVector& SpawnLocation) const  // Spawns the character hit particle system at the given location
 {
 UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), CharacterHitParticles, SpawnLocation);
 }

 void UAGPGameInstance::SpawnTerrainHitParticles(const FVector& SpawnLocation) const  // Spawns the terrain hit particle system at the given location
 {
 UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TerrainHitParticles, SpawnLocation);
 }

 void UAGPGameInstance::SpawnBulletParticles(const FVector& SpawnLocation, const FRotator& SpawnRotation) const  // Spawns the bullet particle system at the given location with the given rotation
 {
 UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BulletParticles, SpawnLocation, SpawnRotation);
 }

 void UAGPGameInstance::PlayGunshotSoundAtLocation(const FVector& Location) const  // Plays the gunshot sound cue at the given location
 {
 UGameplayStatics::PlaySoundAtLocation(GetWorld(), GunshotSoundCue, Location, FRotator::ZeroRotator);
 }

 void UAGPGameInstance::PlayGunshotSound2D() const  // Plays the gunshot sound without a direction, used for the character that fires the gun
 {
 UGameplayStatics::PlaySound2D(GetWorld(), GunshotSoundCue);
 }

 void UAGPGameInstance::PlayMeleeHitSoundAtLocation(const FVector& Location) const  // Plays the melee hit sound at the given location
 {
 UGameplayStatics::PlaySoundAtLocation(GetWorld(), MeleeHitSoundCue, Location, FRotator::ZeroRotator);
 }


