// Fill out your copyright notice in the Description page of Project Settings.


#include "WaveSpawnSubsystem.h"

#include "AGPGameInstance.h"
#include "EngineUtils.h"
#include "Characters/PlayerCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

void UWaveSpawnSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	NavigationSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	SpawnWave(); // Begins wave 1 when the game starts
}

bool UWaveSpawnSubsystem::SpawnEnemyGroup() // Spawns a group of enemies at a random location in the level
{
	FNavLocation GroupSpawnLocation;
	NavigationSystem->GetRandomReachablePointInRadius(FVector(0,0,200), 10000, GroupSpawnLocation); // Sets GroupSpawnLocation to a random reachable point in a large radius around the level
	TArray<AActor*> OutActors;
	TArray<AActor*> IgnoreActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;
	TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel1)); // Custom trace that is only blocked by player characters
	UKismetSystemLibrary::SphereOverlapActors(GetWorld(),GroupSpawnLocation, 2000, TraceObjectTypes, nullptr, IgnoreActors, OutActors); // Checks for any players in a 2000cm radius around the group spawn location
	if (OutActors.Num() > 0) return false; // If there are any players in the radius, return false
	if (const UAGPGameInstance* GameInstance = GetWorld()->GetGameInstance<UAGPGameInstance>())
	{
		for (int EnemyNum = 0; EnemyNum < 5; EnemyNum++) // Each group consists of 5 enemies
		{
			FNavLocation EnemySpawnLocation;
			NavigationSystem->GetRandomReachablePointInRadius(GroupSpawnLocation, 500, EnemySpawnLocation); // Gets a random reachable point in a 5m radius around the group spawn location
			FVector EnemySpawnVector = EnemySpawnLocation.Location; // Converting the enemy spawn location to a FVector
			GetWorld()->SpawnActor<AEnemyCharacter>(GameInstance->GetEnemyCharacterClass(), EnemySpawnVector, FRotator::ZeroRotator); // Spawn an enemy at the enemy spawn location
			RemainingEnemies++; // Increment the remaining enemies counter
		}
		return true; // Return true after spawning all the enemies in the group
	}
	return false; // If no AGP Game instance, return false
}

void UWaveSpawnSubsystem::SpawnWave() // Spawn a whole wave of enemies
{
	if (GetWorld()->GetNetMode() >= NM_Client) return; // Only spawn enemies on the server 
	WaveNumber++; // Increment the wave counter
	
	for (int GroupNum = 0; GroupNum < WaveNumber; GroupNum++) // The number of groups of enemies spawned is the same as the wave number
	{
		while (!SpawnEnemyGroup()); // Keep trying to spawn a group until successful
	}
	UpdateWaveAndEnemyCount(); // Updates the wave and enemy text on the player HUD
}

void UWaveSpawnSubsystem::DecrementEnemyCount() // Decrements the enemy counter
{
	if (GetWorld()->GetNetMode() >= NM_Client) return; // Only do it on the server
	RemainingEnemies--; // Decrement enemy counter
	UpdateWaveAndEnemyCount(); // Updates the wave and enemy text on the player HUD
	if (RemainingEnemies == 0) // If there are no enemies left in the wave i.e. the wave is complete
	{
		// Spawn the next wave after 10 seconds
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UWaveSpawnSubsystem::SpawnWave, 10, false);
	}
}

void UWaveSpawnSubsystem::UpdateWaveAndEnemyCount() // Updates the wave and enemy text on the player HUD
{
	for (TActorIterator<APlayerCharacter> It(GetWorld()); It; ++It) // For each player in the level
	{
		It->MulticastUpdateWaveAndEnemies(WaveNumber, RemainingEnemies); // Update the wave and enemy text on the player HUD
	}
}
