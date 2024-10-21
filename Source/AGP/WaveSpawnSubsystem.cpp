// Fill out your copyright notice in the Description page of Project Settings.


#include "WaveSpawnSubsystem.h"

#include "AGPGameInstance.h"

void UWaveSpawnSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	SpawnWave();
}

void UWaveSpawnSubsystem::SpawnWave()
{
	WaveNumber++;
	UE_LOG(LogTemp, Error, TEXT("WAVE %d"), WaveNumber)
	if (const UAGPGameInstance* GameInstance = GetWorld()->GetGameInstance<UAGPGameInstance>())
	{
		GetWorld()->SpawnActor<AEnemyCharacter>(GameInstance->GetEnemyCharacterClass(), FVector(0,0,200), FRotator::ZeroRotator);
	}
}

void UWaveSpawnSubsystem::DecrementEnemyCount()
{
	RemainingEnemies--;

	// wait x seconds then spawn new wave
}

