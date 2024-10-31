 // Fill out your copyright notice in the Description page of Project Settings.
#include "AGPGameInstance.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Pickups/WeaponPickup.h"

UClass* UAGPGameInstance::GetWeaponPickupClass() const
{
 return WeaponPickupClass.Get();
}

 UClass* UAGPGameInstance::GetEnemyCharacterClass() const
 {
 return EnemyCharacterClass.Get();
 }

 UClass* UAGPGameInstance::GetAlliedCharacterClass() const
 {
 return AlliedCharacterClass.Get();
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

 void UAGPGameInstance::PlayGunshotSoundAtLocation(const FVector& Location)
 {
 UGameplayStatics::PlaySoundAtLocation(GetWorld(), GunshotSoundCue, Location, FRotator::ZeroRotator);
 }

 void UAGPGameInstance::PlayGunshotSound2D()
 {
 UGameplayStatics::PlaySound2D(GetWorld(), GunshotSoundCue);
 }

 void UAGPGameInstance::PlayMeleeHitSoundAtLocation(const FVector& Location)
 {
 UGameplayStatics::PlaySoundAtLocation(GetWorld(), MeleeHitSoundCue, Location, FRotator::ZeroRotator);
 }


