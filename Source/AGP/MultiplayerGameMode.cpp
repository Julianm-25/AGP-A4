// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerGameMode.h"

void AMultiplayerGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if(HasAuthority())
	{
		if(AllySpawnSubsystem)
		{
			AllySpawnSubsystem->UpdatePlayerCount(GetNumPlayers());
		}
	}
}

void AMultiplayerGameMode::BeginPlay()
{
	Super::BeginPlay();
	AllySpawnSubsystem = GetWorld()->GetSubsystem<UAllySpawnSubsystem>();
}

void AMultiplayerGameMode::StartPlay()
{
	Super::StartPlay();
	if(HasAuthority())
	{
		if(AllySpawnSubsystem)
		{
			AllySpawnSubsystem->UpdatePlayerCount(GetNumPlayers());
		}
	}
}
