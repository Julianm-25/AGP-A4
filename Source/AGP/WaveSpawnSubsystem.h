// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NavigationSystem.h"
#include "Subsystems/WorldSubsystem.h"
#include "WaveSpawnSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class AGP_API UWaveSpawnSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

protected:
	int32 WaveNumber = 0; // The current wave of enemies
    int32 RemainingEnemies = 0; // The number of enemies remaining in the wave
	bool SpawnEnemyGroup(); // Spawns a group of enemies at a random location in the level
	void UpdateWaveAndEnemyCount(); // Updates the wave and enemy text on the player HUD

private:
	void SpawnWave(); // Spawn a whole wave of enemies
	UPROPERTY() UNavigationSystemV1* NavigationSystem; // Used to access the navmesh of the level
	
public:
	void DecrementEnemyCount(); // Decrements the enemy counter


	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	
};
