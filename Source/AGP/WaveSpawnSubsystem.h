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
	int32 WaveNumber = 0;
	int32 RemainingEnemies = 0;
	bool SpawnEnemyGroup();
	
private:
	void SpawnWave();
	UPROPERTY() UNavigationSystemV1* NavigationSystem;
	
public:
	void DecrementEnemyCount();

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

};
