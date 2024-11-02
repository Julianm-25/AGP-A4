// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"

#include "BaseCharacter.h"
#include "PlayerCharacter.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	// ...
}

bool UHealthComponent::IsDead()
{
	return bIsDead;
}

float UHealthComponent::GetCurrentHealth() const
{
	return CurrentHealth;
}

float UHealthComponent::GetCurrentHealthPercentage() const
{
	return CurrentHealth / MaxHealth;
}

void UHealthComponent::ApplyDamage(float DamageAmount)
{
	if (bIsDead) return;
	CurrentHealth -= DamageAmount;
	if (CurrentHealth <= 0.0f)
	{
		OnDeath();
		CurrentHealth = 0.0f;
	}
	UpdateHealthBar();
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwner())) // If a player takes damage
	{
		Player->PlayDamageAnimation(); // Play the player HUD damage animation
	}
}

void UHealthComponent::ApplyHealing(float HealingAmount)
{
	if (bIsDead) return;
	CurrentHealth += HealingAmount;
	if (CurrentHealth > 100.0f)
	{
		CurrentHealth = 100.0f;
	}
	UpdateHealthBar();
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UHealthComponent, MaxHealth); // Replicate max health 
	DOREPLIFETIME(UHealthComponent, CurrentHealth); // and current health
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	CurrentHealth = MaxHealth;
}


void UHealthComponent::OnDeath()
{
	bIsDead = true;
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwner()))
	{
		Player->MulticastKillPlayer(); // If a player dies, kill them across all clients
	}
	else if (ABaseCharacter* Character = Cast<ABaseCharacter>(GetOwner()))
	{
		Character->Ragdoll(); // If a character dies, ragdoll them
	}
}

void UHealthComponent::UpdateHealthBar() // Updates the player HUD health bar based on current health percentage
{
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwner()))
	{
		Player->UpdateHealthBar(GetCurrentHealthPercentage());
	}
}

// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

