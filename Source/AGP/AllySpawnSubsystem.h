// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/AlliedCharacter.h"
#include "Subsystems/WorldSubsystem.h"
#include "AllySpawnSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class AGP_API UAllySpawnSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
protected:
	int32 NumPlayers;
	UPROPERTY()TArray<AAlliedCharacter*> Allies;
	void SpawnAllies();
private:
public:
	void UpdatePlayerCount(int32 num);
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
};
