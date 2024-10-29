// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAICharacter.h"
#include "EnemyCharacter.generated.h"

// Forward declarations to avoid needing to #include files in the header of this class.
// When these classes are used in the .cpp file, they are #included there.
class UPawnSensingComponent;
class APlayerCharacter;
class AAIController;

/**
 * An enum to hold the current state of the enemy character.
 */
UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Patrol,
	Engage,
	Evade,
	Investigate,
	Follow
};

/**
 * A class representing the logic for an AI controlled enemy character. 
 */
UCLASS()
class AGP_API AEnemyCharacter : public ABaseAICharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	/**
	 * Logic that controls the enemy character when in the Patrol state.
	 */
	void TickPatrol();
	/**
	 * Logic that controls the enemy character when in the Engage state.
	 */
	void TickEngage() const;
	/**
	 * Logic that controls the enemy character when in the Evade state.
	 */
	void TickEvade();
	/**
	* Logic that controls the enemy character when in the Investigate state.
	*/
	void TickInvestigate() const;
	/**
	 * Logic that controls the enemy character when in the Follow state.
	 */
	void TickFollow() const;

	/**
	 * The current state of the enemy character. This determines which logic to use when executing the finite state machine
	 * found in the tick function of this enemy character.
	 */
	UPROPERTY(EditAnywhere)
	EEnemyState CurrentState = EEnemyState::Patrol;
	int StuckCounter = 0; // Used to stop the enemy from getting stuck for too long when moving
	bool bIsWaiting = false; // Indicates whether the enemy is waiting before moving to the next target location
	float WaitTimer = 0; // How long the enemy has left to wait before moving again
	void Communicate(float CommunicationRadius); // Shares information about player location with nearby enemies
	bool bIsCommander = false; // Indicates whether the enemy is a Commander or not
	UPROPERTY() TWeakObjectPtr<AEnemyCharacter> Commander = nullptr; // A pointer to the commander that this enemy is following
	TArray<TWeakObjectPtr<AEnemyCharacter>> Followers; // Array of enemies currently following this enemy if this enemy is a Commander
	
public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
