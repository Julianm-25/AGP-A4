// Fill out your copyright notice in the Description page of Project Settings.


#include "AlliedCharacter.h"

#include "HealthComponent.h"
#include "AGP/Pickups/WeaponPickup.h"
#include "Perception/PawnSensingComponent.h"

AAlliedCharacter::AAlliedCharacter()
{
	PawnSensingComponent->bOnlySensePlayers = false;
}

void AAlliedCharacter::TickSearch()
{
}

void AAlliedCharacter::TickFollow()
{
}

void AAlliedCharacter::TickHide()
{
}

void AAlliedCharacter::TickHeal()
{
}

void AAlliedCharacter::TickSeekWeapon()
{
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
		LastSeenPlayerLocation = SensedCharacter->GetActorLocation();
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
				CurrentState = EAlliedState::SeekWeapon;
			}
		}
		else if(EnemySeen() && HealthComponent->GetCurrentHealthPercentage() <= 0.4f)
		{
			UE_LOG(LogTemp,Display,TEXT("Hiding"))
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
			CurrentState = EAlliedState::Hide;
		}
		break;
	}
}
