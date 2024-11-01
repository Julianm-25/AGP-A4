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
	UPROPERTY(Replicated)
	int32 WaveNumber = 0;
	UPROPERTY(ReplicatedUsing=UpdateEnemiesLeft)
	int32 RemainingEnemies = 0;
	bool SpawnEnemyGroup();
	
private:
	void SpawnWave();
	UPROPERTY() UNavigationSystemV1* NavigationSystem;
	
public:
	void DecrementEnemyCount();

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION() void UpdateEnemiesLeft();
	
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

};
