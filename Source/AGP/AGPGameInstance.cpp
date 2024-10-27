 // Fill out your copyright notice in the Description page of Project Settings.
#include "AGPGameInstance.h"
#include "NiagaraFunctionLibrary.h"
#include "Pickups/WeaponPickup.h"

UClass* UAGPGameInstance::GetWeaponPickupClass() const
{
 return WeaponPickupClass.Get();
}

 UClass* UAGPGameInstance::GetEnemyCharacterClass() const
 {
 return EnemyCharacterClass.Get();
 }

 void UAGPGameInstance::SpawnMuzzleFlashParticles(const FVector& SpawnLocation, const FRotator& SpawnRotation)
 {
 UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), MuzzleFlashParticles, SpawnLocation, SpawnRotation);
 }

 void UAGPGameInstance::SpawnCharacterHitParticles(const FVector& SpawnLocation)
 {
 UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), CharacterHitParticles, SpawnLocation);
 }

 void UAGPGameInstance::SpawnTerrainHitParticles(const FVector& SpawnLocation)
 {
 UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TerrainHitParticles, SpawnLocation);
 }

 void UAGPGameInstance::SpawnBulletParticles(const FVector& SpawnLocation, const FRotator& SpawnRotation)
 {
 UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BulletParticles, SpawnLocation, SpawnRotation);
 }


