// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAICharacter.h"

#include "EnemyCharacter.h"
#include "NavigationSystem.h"
#include "PlayerCharacter.h"
#include "Perception/PawnSensingComponent.h"

ABaseAICharacter::ABaseAICharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>("Pawn Sensing Component");
}

void ABaseAICharacter::BeginPlay()
{
	Super::BeginPlay();
	
	NavigationSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	//AIController = Cast<AAIController>(GetController());
	
	TargetLocation = GetActorLocation(); // Starting at their Target Location means the enemy's first movement will be delayed based on the random wait time in Patrol, helping to make the movement look more random from the start
	
	if (PawnSensingComponent)
	{
		PawnSensingComponent->OnSeePawn.AddDynamic(this, &ABaseAICharacter::OnSensedPawn);
	}
}

void ABaseAICharacter::OnSensedPawn(APawn* SensedActor)
{
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(SensedActor))
	{
		SensedCharacter = Player;
		//UE_LOG(LogTemp, Display, TEXT("Sensed Player"))
	}
	else if(AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(SensedActor))
	{
		SensedCharacter = Enemy;
		UE_LOG(LogTemp, Display, TEXT("Sensed Enemy"))
	}
}

// Check if the PawnSensingComponent has line of sight to the sensed player
void ABaseAICharacter::UpdateSight()
{
	if (!SensedCharacter.IsValid()) return;
	if (PawnSensingComponent)
	{
		if (!PawnSensingComponent->HasLineOfSightTo(SensedCharacter.Get())) // If there is no line of sight to the player, set the last known location of the player and set SensedCharacter to be a null pointer
		{
			if(APlayerCharacter* Player = Cast<APlayerCharacter>(SensedCharacter.Get()))
			{
				LastSeenPlayerLocation = SensedCharacter->GetActorLocation(); 
			}
			SensedCharacter = nullptr;
			//UE_LOG(LogTemp, Display, TEXT("Lost Player"))
		}
	}
}

// Sets TargetLocation to a random reachable point within a given radius around a given point
void ABaseAICharacter::SetRandomReachableLocationInRadius(const FVector& CenterPoint, const float Radius)
{
	FNavLocation NavLocation;
	NavigationSystem->GetRandomReachablePointInRadius(CenterPoint, Radius, NavLocation); // Gets a random point within the given radius in navigable space that the enemy can reach
	TargetLocation = NavLocation.Location; // Converts the output of GetRandomReachablePointInRadius to a FVector

	/*DrawDebugSphere(GetWorld(), TargetLocation, 5.0f, 8, FColor::Red, false, 5, 0, 5.0f);
	DrawDebugSphere(GetWorld(), CenterPoint, Radius, 20, FColor::Blue, false, 0.5, 0, 5.0f);
	UE_LOG(LogTemp, Display, TEXT("Center Point: %s"), *CenterPoint.ToString());
	UE_LOG(LogTemp, Display, TEXT("Target Location: %s"), *TargetLocation.ToString());*/
}

bool ABaseAICharacter::PlayerSeen()
{
	if (!SensedCharacter.IsValid()) return false;
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(SensedCharacter.Get())) return true;
	return false;
}

bool ABaseAICharacter::EnemySeen()
{
	if (!SensedCharacter.IsValid()) return false;
	if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(SensedCharacter.Get())) return true;
	return false;
}

void ABaseAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABaseAICharacter::DelayedDespawn()
{
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ABaseAICharacter::Despawn, DespawnTimer, false);
}

void ABaseAICharacter::Despawn()
{
	Destroy();
}
