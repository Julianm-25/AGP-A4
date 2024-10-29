// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"

#include "AIController.h"
#include "HealthComponent.h"
#include "PlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;
	//PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>("Pawn Sensing Component");
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	TargetLocation = GetActorLocation(); // Starting at their Target Location means the enemy's first movement will be delayed based on the random wait time in Patrol, helping to make the movement look more random from the start
	if (FMath::RandRange(1,10) == 10) // Every enemy has a 1 in 10 chance to become a Commander when spawned
	{
		bIsCommander = true;
		SetActorScale3D(GetActorScale() * 1.5);
	}
}

void AEnemyCharacter::TickPatrol()
{
	if (bIsWaiting) // If the enemy is waiting, decrement their wait timer
	{
		WaitTimer -= GetWorld()->DeltaTimeSeconds;
		if (WaitTimer <= 0) // If the timer goes to 0, the enemy can begin moving again
		{
			//UE_LOG(LogTemp, Warning, TEXT("WAIT TIME IS OVER"));
			bIsWaiting = false;
		}
	}
	
	else if (FVector::Distance(GetActorLocation(), TargetLocation) < PathfindingError) // Once an enemy gets close enough to its target location
	{
		if (bIsCommander) Communicate(600.0f); // If the enemy is a Commander, it will try to gather more followers (up to 4) each time it reaches its target location
		WaitTimer = FMath::RandRange(1.0f,10.0f); // Wait for 1 to 10 seconds before picking a new patrol point
		bIsWaiting = true;
		SetRandomReachableLocationInRadius(GetActorLocation(), 1000.0f); // Set the next patrol location
		AIController->MoveToLocation(TargetLocation); // Move towards the new target location
	}
	else if (GetVelocity() == FVector::Zero()) // If an enemy is not moving, but isn't waiting, then it is probably stuck on something
	{
		StuckCounter++;
		//UE_LOG(LogTemp, Warning, TEXT("%s IS STUCK"), *GetActorNameOrLabel());
		if (StuckCounter > 100) // If the enemy has been stuck for long enough, give it a new location to move to and reset the stuck counter
		{
			SetRandomReachableLocationInRadius(GetActorLocation(), 1000.0f);
			AIController->MoveToLocation(TargetLocation);
			StuckCounter = 0;
		}
	}
}

void AEnemyCharacter::TickEngage()
{
	if (!SensedCharacter.IsValid()) return;

	if (FVector::Distance(GetActorLocation(), SensedCharacter->GetActorLocation()) < 100.0f && TimeSinceLastAttack >= 3.0f) // If the enemy is close enough to the detected player
	{
		// Melee attack implementation will go here
		StartMeleeAttack();
	}
	else AIController->MoveToActor(SensedCharacter.Get()); // If not yet in melee range, move towards the detected player
}

void AEnemyCharacter::TickEvade()
{
	// Find the player and return if it can't find it.
	if (!SensedCharacter.IsValid()) return;
	
	//UE_LOG(LogTemp, Display, TEXT("Target Location: %s"), *TargetLocation.ToString());
	if (FVector::Distance(GetActorLocation(), TargetLocation) < PathfindingError) // If the enemy is close enough to the target location
	{
		SetRandomReachableLocationInRadius(GetNormalizedEvadeTarget(),300.0f); // Set a new evade location away from the sensed player
		AIController->MoveToLocation(TargetLocation); // Then move to the new target location
	}
	else if (GetVelocity() == FVector::Zero()) // The same unstuck technique as in the Patrol state, though this one has less tolerance for being stuck as Evade should be smoother than Patrol
	{
		StuckCounter++;
		if (StuckCounter > 25)
		{
			SetRandomReachableLocationInRadius(GetActorLocation(), 300.0f);
			AIController->MoveToLocation(TargetLocation);
			StuckCounter = 0;
		}
	}
}

void AEnemyCharacter::TickInvestigate() const
{
	AIController->MoveToLocation(LastSeenPlayerLocation); // Move towards the place where this enemy last saw the player
}

void AEnemyCharacter::TickFollow() const
{
	if (!Commander.IsValid()) return;
	if (FVector::Distance(GetActorLocation(), Commander->GetActorLocation()) > 500.0f) AIController->MoveToActor(Commander.Get()); // If more than 500 cm away from the Commander, move towards the commander
}

// Shares information about player location with nearby enemies
void AEnemyCharacter::Communicate(float CommunicationRadius)
{
	TArray<AActor*> OutEnemies; // Array of enemies within range of the communicating enemy
	TArray<AActor*> IgnoreEnemies; // Array of actors to ignore when checking for nearby enemies
	IgnoreEnemies.Add(this); // Adds self to be ignored, as the communicating enemy is already aware of the player's position
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes; // Determines which traces will be checked when checking for actors
	TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn)); // Adds the Pawn Trace Channel to be checked by the overlap sphere
	UKismetSystemLibrary::SphereOverlapActors(GetWorld(),GetActorLocation(), CommunicationRadius, TraceObjectTypes, nullptr, IgnoreEnemies, OutEnemies); // Checks all Pawns within a certain radius and adds the actors to the OutEnemies array
	//DrawDebugSphere(GetWorld(), GetActorLocation(),CommunicationRadius,20,FColor::Emerald,false,1,0,2);
	for (auto OutEnemy : OutEnemies) // For each Actor detected by the overlap sphere
	{
		if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(OutEnemy)) // If the Actor is an enemy
		{
			if (bIsCommander && !Enemy->bIsCommander) // If THIS enemy is a Commander and the detected enemy is not a Commander
			{
				if (Enemy->Commander == nullptr && Followers.Num() < 4) // If the detected enemy does not have a Commander, and THIS enemy has less than 4 followers
				{
					Enemy->Commander = this; // Set the detected enemy to be a follower of THIS enemy
					Followers.Add(Enemy);
					Enemy->CurrentState = EEnemyState::Follow;
					//UE_LOG(LogTemp, Display, TEXT("ADDED ENEMY TO FOLLOWERS"));
				}
			}
			else if (!Enemy->Commander.IsValid() || !Enemy->bIsCommander) // if the detected enemy isn't a Commander or a follower
			{
				if (CurrentState == EEnemyState::Investigate && Enemy->CurrentState == EEnemyState::Patrol) // If the communicating enemy is investigating and the detected enemy is patrolling
				{
					Enemy->LastSeenPlayerLocation = LastSeenPlayerLocation; // Share the last seen location of the player
					Enemy->CurrentState = EEnemyState::Investigate; // And set the detected enemy to investigate
					//UE_LOG(LogTemp, Display, TEXT("COMMUNICATED INVESTIGATE LOCATION"));
				}
				else if (CurrentState == EEnemyState::Engage && (Enemy->CurrentState == EEnemyState::Patrol || Enemy->CurrentState == EEnemyState::Investigate)) // If the communicating enemy is engaging the player and the detected enemy is patrolling or investigating
				{
					Enemy->SensedCharacter = SensedCharacter; // Alert the detected enemy to the player that is being engaged, this also causes the detected enemy to engage the player
					//UE_LOG(LogTemp, Display, TEXT("COMMUNICATED ENGAGE LOCATION"));
				}	
			}
		}
	}
}


void AEnemyCharacter::StartMeleeAttack()
{
	if (!SensedCharacter.IsValid()) return;
	TimeSinceLastAttack = 0;
	UE_LOG(LogTemp, Display, TEXT("STARTING ATTACK"));
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AEnemyCharacter::FinishMeleeAttack, 1, false);
}

void AEnemyCharacter::FinishMeleeAttack()
{
	if (!SensedCharacter.IsValid() || FVector::Distance(GetActorLocation(), SensedCharacter.Get()->GetActorLocation()) > 100.0f) return;
	if (UHealthComponent* HitCharacterHealth = SensedCharacter.Get()->GetComponentByClass<UHealthComponent>())
	{
		HitCharacterHealth->ApplyDamage(10); // Arbitrary damage value
		UE_LOG(LogTemp, Display, TEXT("FINISHING ATTACK"));
	}
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	UpdateSight();
	TimeSinceLastAttack += GetWorld()->DeltaTimeSeconds;
	switch(CurrentState)
	{
	case EEnemyState::Patrol:
		TickPatrol();
		if (SensedCharacter.IsValid())
		{
			if ((HealthComponent->GetCurrentHealthPercentage() >= 0.4f && !Commander.IsValid()) || bIsCommander) // If the enemy has more than 40% health and isn't a follower, or if the enemy is a Commander
			{
				//UE_LOG(LogTemp, Display, TEXT("SENSED PLAYER, SHOULD ENGAGE"));
				bIsWaiting = false; // End any wait that is currently happening
				CurrentState = EEnemyState::Engage; // Engage the player
				Communicate(300.0f); // Communicate the engaged player's location with any nearby enemies
				if (bIsCommander) // If this enemy is a Commander, set all follower's sensed character to the player being engaged
				{
					for (auto Follower : Followers)
					{
						Follower->SensedCharacter = SensedCharacter.Get();
					}
				}
			}
			else // If below 40% health, run away from the player
			{
				bIsWaiting = false;
				SetRandomReachableLocationInRadius(GetNormalizedEvadeTarget(),500.0f);
				CurrentState = EEnemyState::Evade;
			}
		}
		break;
	case EEnemyState::Engage:
		TickEngage();
		if (HealthComponent->GetCurrentHealthPercentage() < 0.4f && !Commander.IsValid() && !bIsCommander) // Commanders and enemies following a Commander will not try to run from a player
		{
			SetRandomReachableLocationInRadius(GetNormalizedEvadeTarget() ,350.0f);
			AIController->MoveToLocation(TargetLocation);
			CurrentState = EEnemyState::Evade;
		} else if (!SensedCharacter.IsValid())
		{
			if (Commander.IsValid()) // If the enemy has a Commander, go back to following it
			{
				CurrentState = EEnemyState::Follow;
			}
			else // If the enemy loses sight of a player while engaged, go to the last known location of the player
			{
				CurrentState = EEnemyState::Investigate;
				Communicate(300.0f);
			}
		}
		break;
	case EEnemyState::Evade:
		TickEvade();
		if (HealthComponent->GetCurrentHealthPercentage() >= 0.4f) // If health goes back above 40%, re-engage the player
		{
			CurrentState = EEnemyState::Engage;
			Communicate(300.0f);
		} else if (!SensedCharacter.IsValid()) // If the enemy loses sight of the player, patrol
		{
			CurrentState = EEnemyState::Patrol;
		}
		break;
	case EEnemyState::Investigate:
		TickInvestigate();
		if (SensedCharacter.IsValid()) // If a player is detected while investigating, engage them
		{
			CurrentState = EEnemyState::Engage;
			bIsWaiting = false;
			Communicate(300.0f);
		}
		else if (FVector::Distance(GetActorLocation(), LastSeenPlayerLocation) < PathfindingError) // The enemy waits for 5 seconds before returning to patrol once it reaches the last known player location
		{
			SetRandomReachableLocationInRadius(GetActorLocation(), 1000.0f);
			WaitTimer = 5;
			bIsWaiting = true;
			CurrentState = EEnemyState::Patrol;
		}
		break;
	case EEnemyState::Follow:
		TickFollow();
		if (SensedCharacter.IsValid()) // If a player is detected, engage them
		{
			CurrentState = EEnemyState::Engage;
		}
		else if (!Commander.IsValid()) // If the Commander dies and no player is detected, go back to patrolling
		{
			CurrentState = EEnemyState::Patrol;
		}
		break;
	}
}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


