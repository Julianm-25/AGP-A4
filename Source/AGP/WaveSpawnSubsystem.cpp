// Fill out your copyright notice in the Description page of Project Settings.


#include "WaveSpawnSubsystem.h"

#include "AGPGameInstance.h"
#include "EngineUtils.h"
#include "Kismet/KismetSystemLibrary.h"

void UWaveSpawnSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	NavigationSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	SpawnWave();
}

bool UWaveSpawnSubsystem::SpawnEnemyGroup()
{
	FNavLocation GroupSpawnLocation;
	NavigationSystem->GetRandomReachablePointInRadius(FVector(0,0,200), 10000, GroupSpawnLocation);
	TArray<AActor*> OutActors;
	TArray<AActor*> IgnoreActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;
	TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel1));
	UKismetSystemLibrary::SphereOverlapActors(GetWorld(),GroupSpawnLocation, 1000, TraceObjectTypes, nullptr, IgnoreActors, OutActors);
	if (OutActors.Num() > 0) return false;
	if (const UAGPGameInstance* GameInstance = GetWorld()->GetGameInstance<UAGPGameInstance>())
	{
		for (int EnemyNum = 0; EnemyNum < 5; EnemyNum++)
		{
			FNavLocation EnemySpawnLocation;
			NavigationSystem->GetRandomReachablePointInRadius(GroupSpawnLocation, 300, EnemySpawnLocation);
			/*TArray<AActor*> OutEnemies;
			TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
			UKismetSystemLibrary::SphereOverlapActors(GetWorld(),EnemySpawnLocation, 20, TraceObjectTypes, nullptr, IgnoreActors, OutEnemies);
			if(OutEnemies.Num() > 0)
			{
				EnemyNum--;
				UE_LOG(LogTemp, Warning, TEXT("FAILED SPAWN"))
				continue;
			}*/
			FVector EnemySpawnVector = EnemySpawnLocation.Location;
			EnemySpawnVector.Z += 25; // Arbitrary value to stop them from being stuck in the ground
			GetWorld()->SpawnActor<AEnemyCharacter>(GameInstance->GetEnemyCharacterClass(), EnemySpawnVector, FRotator::ZeroRotator);
			RemainingEnemies++;
			UE_LOG(LogTemp, Log, TEXT("SUCCESSFUL SPAWN"))
		}
		return true;
	}
	return false;	
}

void UWaveSpawnSubsystem::SpawnWave()
{
	WaveNumber++;
	
	UE_LOG(LogTemp, Log, TEXT("WAVE %d"), WaveNumber)

	for (int GroupNum = 0; GroupNum < WaveNumber; GroupNum++)
	{
		while (!SpawnEnemyGroup());
	}
	for (TActorIterator<APlayerCharacter> It(GetWorld()); It; ++It)
	{
		It->UpdateWaveCount(WaveNumber);
		It->UpdateEnemiesLeftCount(RemainingEnemies);
	}
}

void UWaveSpawnSubsystem::DecrementEnemyCount()
{
	RemainingEnemies--;
	for (TActorIterator<APlayerCharacter> It(GetWorld()); It; ++It)
	{
		It->UpdateEnemiesLeftCount(RemainingEnemies);
	}
	// wait x seconds then spawn new wave
	if (RemainingEnemies == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("WAVE COMPLETED"))
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UWaveSpawnSubsystem::SpawnWave, 10, false);
	}
}