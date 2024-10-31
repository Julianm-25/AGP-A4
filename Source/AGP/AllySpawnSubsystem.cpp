// Fill out your copyright notice in the Description page of Project Settings.


#include "AllySpawnSubsystem.h"

#include "AGPGameInstance.h"

class UAGPGameInstance;

void UAllySpawnSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	//SpawnAllies();
}

bool UAllySpawnSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer))
	{
		return false;
	}

	UWorld* World = Cast<UWorld>(Outer);
	return World->GetNetMode() < NM_Client; // will be created in standalone, dedicated servers, and listen servers
}

void UAllySpawnSubsystem::SpawnAllies()
{
	UE_LOG(LogTemp, Log, TEXT("Spawn Allies Called"))
	//Get number of players
	if(NumPlayers + Allies.Num() > 4)
	{
		UE_LOG(LogTemp, Log, TEXT("Allies being destroyed"))
		while(NumPlayers + Allies.Num() > 4)
		{
			UE_LOG(LogTemp, Log, TEXT(" Before Destroy Current number of allies: %i"),Allies.Num())
			Allies.Pop()->Despawn();
			UE_LOG(LogTemp, Log, TEXT("After Pop Current number of allies: %i"),Allies.Num())
		}
	}
	else if(NumPlayers + Allies.Num() < 4)
	{
		UE_LOG(LogTemp, Log, TEXT("Allies being created"))
		if (const UAGPGameInstance* GameInstance = GetWorld()->GetGameInstance<UAGPGameInstance>())
		{
			while(NumPlayers + Allies.Num() < 4)
			{
				UE_LOG(LogTemp, Log, TEXT("Before Spawn Current number of allies: %i"),Allies.Num())
				FVector *AllySpawnVector = new FVector(-1500.0f,4600.0f,25.0f);
				//AllySpawnVector.Z += 25; // Arbitrary value to stop them from being stuck in the ground
				Allies.Add(GetWorld()->SpawnActor<AAlliedCharacter>(GameInstance->GetAlliedCharacterClass(), *AllySpawnVector, FRotator::ZeroRotator));
				UE_LOG(LogTemp, Log, TEXT("After Spawn Current number of allies: %i"),Allies.Num())
			}
		}
	}

}

void UAllySpawnSubsystem::UpdatePlayerCount(int32 num)
{
	NumPlayers = num;
	UE_LOG(LogTemp,Display,TEXT("Number of players: %i"), NumPlayers)
	SpawnAllies();
}
