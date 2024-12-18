// Fill out your copyright notice in the Description page of Project Settings.


#include "WaveSpawnSubsystem.h"

#include "AGPGameInstance.h"
#include "EngineUtils.h"
#include "Characters/PlayerCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"


void UWaveSpawnSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	NavigationSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	SpawnWave();
}

void UWaveSpawnSubsystem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UWaveSpawnSubsystem, WaveNumber);
	DOREPLIFETIME(UWaveSpawnSubsystem, RemainingEnemies);
}

void UWaveSpawnSubsystem::UpdateEnemiesLeft()
{
	for (TActorIterator<APlayerCharacter> It(GetWorld()); It; ++It)
	{
		It->UpdateEnemiesLeftCount(RemainingEnemies);
	}
}

bool UWaveSpawnSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer))
	{
		return false;
	}

	UWorld* World = Cast<UWorld>(Outer);
	return World->GetNetMode() < NM_Client; // will be created in standalone, dedicated servers, and listen servers
}

bool UWaveSpawnSubsystem::SpawnEnemyGroup()
{
	if (GetWorld()->GetNetMode() == NM_Client) return false;
	FNavLocation GroupSpawnLocation;
	NavigationSystem->GetRandomReachablePointInRadius(FVector(0,0,200), 10000, GroupSpawnLocation);
	TArray<AActor*> IgnoreActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;
	TArray<AActor*> OutActors;
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
		UpdateEnemiesLeft();
		return true;
	}
	return false;	
}

void UWaveSpawnSubsystem::SpawnWave()
{
	if (GetWorld()->GetNetMode() == NM_Client) return;
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
	UpdateEnemiesLeft();
	// wait x seconds then spawn new wave
	if (RemainingEnemies == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("WAVE COMPLETED"))
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UWaveSpawnSubsystem::SpawnWave, 10, false);
	}
}