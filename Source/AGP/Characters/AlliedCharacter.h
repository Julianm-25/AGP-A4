// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAICharacter.h"
#include "BaseCharacter.h"
#include "AGP/Pickups/WeaponPickup.h"
#include "AlliedCharacter.generated.h"

//Forward declarations
enum class EWeaponRarity : uint8;


/**
 * An enum to hold the current state of the Ally AI
 */
UENUM(BlueprintType)
enum class EAlliedState : uint8
{
	Search,
	Follow,
	Hide,
	Heal,
	SeekWeapon
};


/**
 * A class representing the logic for an AI controlled ally. 
 */
UCLASS()
class AGP_API AAlliedCharacter : public ABaseAICharacter
{
	GENERATED_BODY()

public:
	AAlliedCharacter();

protected:

	//Tick Functions for each State
	void TickSearch();
	void TickFollow();
	void TickHide();
	void TickHeal();
	void TickSeekWeapon();

	//Helper Functions
	bool WeaponIsHigherRarity();
	bool PlayerNeedsHealing();
	bool PlayerReadyForWeapon();
	
	
	//Variables for Healing Functionality
	UPROPERTY(EditAnywhere)
	float HealDistance = 300.0f;
	UPROPERTY(EditAnywhere)
	float HealCooldown = 3.0f;
	UPROPERTY(EditAnywhere)
	float HealAmount = 10.0f;
	float TimeSinceLastHeal = 0.0f;

	//Variables and Functions for Weapon Delivery Functionality
	UPROPERTY(EditAnywhere)
	float WeaponDeliveryDistance = 300.0f;
	UPROPERTY(EditAnywhere)
	float WeaponDeliveryCooldown = 30.0f;
	float TimeSinceLastDelivery = 0.0f;
	EWeaponRarity PlayerWeaponRarity = EWeaponRarity::None;
	void DeliverWeapon();
	
	UPROPERTY(EditAnywhere)
	EAlliedState CurrentState = EAlliedState::Search;
	//FVector LocationWhenPlayerLost;
	FVector PlayerLastLocation;
	bool bTracedSteps = true;

public:	
	virtual void Tick(float DeltaTime) override;
	
};
