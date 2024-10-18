// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BaseCharacter.h"
#include "NavigationSystem.h"
#include "PlayerCharacter.h"
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
class AGP_API AEnemyCharacter : public ABaseCharacter
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
	 * A function bound to the UPawnSensingComponent's OnSeePawn event. This will set the SensedCharacter variable
	 * if the pawn that was sensed was of type APlayerCharacter.
	 * @param SensedActor The pawn that was sensed by the UPawnSensingComponent.
	 */
	UFUNCTION()
	void OnSensedPawn(APawn* SensedActor);
	/**
	 * Will update the SensedCharacter variable based on whether the UPawnSensingComponent has a line of sight to the
	 * Player Character or not. This may cause the SensedCharacter variable to become a nullptr so be careful when using
	 * the SensedCharacter variable.
	 */
	void UpdateSight();
	
	/**
	 * A pointer to the PawnSensingComponent attached to this enemy character.
	 */
	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensingComponent;

	/**
	 * A pointer to a PlayerCharacter that can be seen by this enemy character. If this is nullptr then the enemy cannot
	 * see any PlayerCharacter.
	 */
	UPROPERTY()
	APlayerCharacter* SensedCharacter = nullptr;

	/**
	 * The current state of the enemy character. This determines which logic to use when executing the finite state machine
	 * found in the tick function of this enemy character.
	 */
	UPROPERTY(EditAnywhere)
	EEnemyState CurrentState = EEnemyState::Patrol;

	/**
	 * Some arbitrary error value for determining how close is close enough before moving onto the next step in the path.
	 */
	UPROPERTY(EditAnywhere)
	float PathfindingError = 150.0f; // 150 cm from target by default.
	FVector TargetLocation; // Targeted location for the enemy to move towards
	void SetRandomReachableLocationInRadius(const FVector& CenterPoint, const float Radius); // Sets TargetLocation to a random location around a given point that the enemy can reach
	UPROPERTY() AAIController* AIController; // The AIController class allows the enemy to move towards given locations
	FVector GetNormalizedEvadeTarget() const; // Used to find the direction away from the player when the enemy is evading
	int StuckCounter = 0; // Used to stop the enemy from getting stuck for too long when moving
	bool bIsWaiting = false; // Indicates whether the enemy is waiting before moving to the next target location
	float WaitTimer = 0; // How long the enemy has left to wait before moving again
	void Communicate(float CommunicationRadius); // Shares information about player location with nearby enemies
	bool bIsCommander = false; // Indicates whether the enemy is a Commander or not
	UPROPERTY() AEnemyCharacter* Commander = nullptr; // A pointer to the commander that this enemy is following
	TArray<AEnemyCharacter*> Followers; // Array of enemies currently following this enemy if this enemy is a Commander
public:	

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	FVector LastSeenPlayerLocation; // The location where the enemy last saw the player
private:
	
	UPROPERTY() UNavigationSystemV1* NavigationSystem; // The navigation system used by the enemy to determine where it can move
};
