// Fill out your copyright notice in the Description page of Project Settings.


#include "AlliedCharacter.h"

#include "AIController.h"
#include "HealthComponent.h"
#include "AGP/Pickups/WeaponPickup.h"
#include "Kismet/KismetMathLibrary.h"
#include "Perception/PawnSensingComponent.h"

AAlliedCharacter::AAlliedCharacter()
{
	PawnSensingComponent->bOnlySensePlayers = false;
}

void AAlliedCharacter::TickSearch()
{
	
	if(!bTracedSteps)
	{
		if(FVector::Distance(GetActorLocation(), LastSeenPlayerLocation) < 100)
		{
			UE_LOG(LogTemp,Display,TEXT("Steps have been Traced"))
			bTracedSteps = true;
			SetRandomReachableLocationInRadius(GetActorLocation(), 1000.0f);
			AIController->MoveToLocation(TargetLocation);
		}
		else
		{
			UE_LOG(LogTemp,Display,TEXT("Moving Towards Players last seen location: %f"), FVector::Distance(GetActorLocation(),LastSeenPlayerLocation))
			AIController->MoveToLocation(LastSeenPlayerLocation);
		}
	}
	else
	{
		if (FVector::Distance(GetActorLocation(), TargetLocation) < PathfindingError)
		{
			UE_LOG(LogTemp,Display,TEXT("Arrived at random location, generating new location"))
			SetRandomReachableLocationInRadius(GetActorLocation(), 1000.0f);
			AIController->MoveToLocation(TargetLocation);
		}
	}
	
}

void AAlliedCharacter::TickFollow()
{
	if (!SensedCharacter.IsValid()) return;
	AIController->MoveToActor(SensedCharacter.Get());
}

void AAlliedCharacter::TickHide()
{
	// Find the player and return if it can't find it.
	if (!SensedCharacter.IsValid()) return;
	
	//UE_LOG(LogTemp, Display, TEXT("Target Location: %s"), *TargetLocation.ToString());
	if (FVector::Distance(GetActorLocation(), TargetLocation) < PathfindingError) // If the enemy is close enough to the target location
	{
		SetRandomReachableLocationInRadius(GetNormalizedEvadeTarget(),300.0f); // Set a new evade location away from the sensed player
		AIController->MoveToLocation(TargetLocation); // Then move to the new target location
	}
}

void AAlliedCharacter::TickHeal()
{
	if (PlayerSeen())
	{
		if(TimeSinceLastHeal > HealCooldown)
		{
			SensedCharacter->GetComponentByClass<UHealthComponent>()->ApplyHealing(HealAmount);
			TimeSinceLastHeal = 0.0f;
		}
	}
}

void AAlliedCharacter::TickSeekWeapon()
{
	LookForWeapon();
	if(bWeaponSpotted && !WeaponIsHigherRarity())
	{
		AIController->MoveToLocation(WeaponLocation);
		if(FVector::Distance(GetActorLocation(),WeaponLocation) < 50)
		{
			SetRandomReachableLocationInRadius(GetActorLocation(), 1000.0f);
			AIController->MoveToLocation(TargetLocation);
			bWeaponSpotted = false;
			UE_LOG(LogTemp,Display,TEXT("Picked up weapon"))
		}
	}
	else if (FVector::Distance(GetActorLocation(), TargetLocation) < PathfindingError)
	{
		SetRandomReachableLocationInRadius(GetActorLocation(), 1000.0f);
		AIController->MoveToLocation(TargetLocation);
	}
}

bool AAlliedCharacter::WeaponIsHigherRarity()
{
	if(HasWeapon() && PlayerWeaponRarity < GetComponentByClass<UWeaponComponent>()->GetWeaponRarity())
	{
		return true;
	}
	return false;
}

bool AAlliedCharacter::PlayerNeedsHealing()
{
	if(SensedCharacter->GetComponentByClass<UHealthComponent>()->GetCurrentHealthPercentage()<=0.9f
		&& FVector::Distance(GetActorLocation(),SensedCharacter->GetActorLocation()) < HealDistance)
	{
		return true;
	}
	return false;
}

bool AAlliedCharacter::PlayerReadyForWeapon()
{
	if(TimeSinceLastDelivery > WeaponDeliveryCooldown
		&& WeaponIsHigherRarity()
		&& FVector::Distance(GetActorLocation(),SensedCharacter->GetActorLocation()) < WeaponDeliveryDistance)
	{
		return true;
	}
	return false;
}

void AAlliedCharacter::DeliverWeapon()
{
	UE_LOG(LogTemp,Display,TEXT("Weapon Delivered of Quality %hhd"), WeaponComponent->GetWeaponRarity())
	SensedCharacter->EquipWeapon(true, WeaponComponent->GetWeaponRarity(), WeaponComponent->GetWeaponStats());
	EquipWeapon(false, EWeaponRarity::None);
	TimeSinceLastDelivery = 0.0f;
}



void AAlliedCharacter::LookForWeapon()
{
	//raycast search for weapon pickup
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation()+ 5000* UKismetMathLibrary::GetForwardVector(GetActorRotation()), FColor::Green, false, 1.0f);
	if (GetWorld()->LineTraceSingleByChannel(HitResult, GetActorLocation(), GetActorLocation()+5000*UKismetMathLibrary::GetForwardVector(GetActorRotation()),ECC_WorldDynamic, QueryParams))
	{
		if(AWeaponPickup *Weapon = Cast<AWeaponPickup>(HitResult.GetActor()))
		{
			UE_LOG(LogTemp,Display,TEXT("Hit a weapon"))
			bWeaponSpotted = true;
			WeaponLocation = HitResult.ImpactPoint;
		}
	}
	//if spotted, set weapon seen to true, start walking towards weapon
	//if not do nothing
}

void AAlliedCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateSight();
	if(PlayerSeen())
	{
		if(SensedCharacter->HasWeapon())
		{
			PlayerWeaponRarity = SensedCharacter->GetComponentByClass<UWeaponComponent>()->GetWeaponRarity();
		}
		bTracedSteps = false;
	}
	TimeSinceLastHeal+= DeltaTime;
	TimeSinceLastDelivery += DeltaTime;
	switch (CurrentState)
	{
	case EAlliedState::Search:
		TickSearch();
		if(TimeSinceLastDelivery > (0.8*WeaponDeliveryCooldown) && PlayerWeaponRarity != EWeaponRarity::Legendary && !WeaponIsHigherRarity())
		{
			UE_LOG(LogTemp,Display,TEXT("Searching For Weapon"))
			SetRandomReachableLocationInRadius(GetActorLocation(), 1000.0f);
			AIController->MoveToLocation(TargetLocation);
			CurrentState = EAlliedState::SeekWeapon;
		}
		else if (PlayerSeen())
		{
			UE_LOG(LogTemp,Display,TEXT("Following"))
			CurrentState = EAlliedState::Follow;
		}
		else if(EnemySeen() && HealthComponent->GetCurrentHealthPercentage() <= 0.6f)
		{
			UE_LOG(LogTemp,Display,TEXT("Hiding"))
			SetRandomReachableLocationInRadius(GetNormalizedEvadeTarget(),300.0f);
			AIController->MoveToLocation(TargetLocation);
			CurrentState = EAlliedState::Hide;
		}
		break;
		
	case EAlliedState::Follow:
		TickFollow();
		if (PlayerSeen())
		{
			if(PlayerReadyForWeapon())
			{
				DeliverWeapon();
			}
			
			if(PlayerNeedsHealing())
			{
				UE_LOG(LogTemp,Display,TEXT("Healing"))
				CurrentState = EAlliedState::Heal;
			}
			else if(TimeSinceLastDelivery > (0.8*WeaponDeliveryCooldown) && PlayerWeaponRarity != EWeaponRarity::Legendary && !WeaponIsHigherRarity())
			{
				UE_LOG(LogTemp,Display,TEXT("Searching For Weapon"))
				SetRandomReachableLocationInRadius(GetActorLocation(), 1000.0f);
				AIController->MoveToLocation(TargetLocation);
				CurrentState = EAlliedState::SeekWeapon;
			}
		}
		else if(EnemySeen() && HealthComponent->GetCurrentHealthPercentage() <= 0.4f)
		{
			UE_LOG(LogTemp,Display,TEXT("Hiding"))
			SetRandomReachableLocationInRadius(GetNormalizedEvadeTarget(),300.0f);
			AIController->MoveToLocation(TargetLocation);
			CurrentState = EAlliedState::Hide;
		}
		else
		{
			UE_LOG(LogTemp,Display,TEXT("Searching"))
			CurrentState = EAlliedState::Search;
		}
		break;
		
	case EAlliedState::Hide:
		TickHide();
		if(!EnemySeen())
		{
			if (PlayerSeen())
			{
				UE_LOG(LogTemp,Display,TEXT("Following"))
				CurrentState = EAlliedState::Follow;
			}
			else
			{
				UE_LOG(LogTemp,Display,TEXT("Searching"))
				CurrentState = EAlliedState::Search;
			}
		}
		break;
		
	case EAlliedState::Heal:
		TickHeal();
		if(PlayerSeen())
		{
			if(SensedCharacter->GetComponentByClass<UHealthComponent>()->GetCurrentHealthPercentage()>=0.9f)
			{
				UE_LOG(LogTemp,Display,TEXT("Following"))
				CurrentState = EAlliedState::Follow;
			}
		}
		else if(EnemySeen() && HealthComponent->GetCurrentHealthPercentage() <= 0.4f)
		{
			UE_LOG(LogTemp,Display,TEXT("Hiding"))
			SetRandomReachableLocationInRadius(GetNormalizedEvadeTarget(),300.0f);
			AIController->MoveToLocation(TargetLocation);
			CurrentState = EAlliedState::Hide;
		}
		else
		{
			UE_LOG(LogTemp,Display,TEXT("Searching"))
			CurrentState = EAlliedState::Search;
		}
		break;
		
	case EAlliedState::SeekWeapon:
		TickSeekWeapon();
		if (PlayerSeen())
		{
			if(WeaponIsHigherRarity())
			{
				UE_LOG(LogTemp,Display,TEXT("Higher Quality Weapon Acquired"))
				CurrentState = EAlliedState::Follow;
			}
			else if(PlayerNeedsHealing())
			{
				UE_LOG(LogTemp,Display,TEXT("Healing"))
				CurrentState = EAlliedState::Heal;
			}
		}
		else if(EnemySeen() && HealthComponent->GetCurrentHealthPercentage() <= 0.4f)
		{
			UE_LOG(LogTemp,Display,TEXT("Hiding"))
			SetRandomReachableLocationInRadius(GetNormalizedEvadeTarget(),300.0f);
			AIController->MoveToLocation(TargetLocation);
			CurrentState = EAlliedState::Hide;
		}
		break;
	}
}

void AAlliedCharacter::BeginPlay()
{
	Super::BeginPlay();
	SetRandomReachableLocationInRadius(GetActorLocation(), 1000.0f);
	AIController->MoveToLocation(TargetLocation);
}
