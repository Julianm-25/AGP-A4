// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "BaseAICharacter.generated.h"

class AAIController;
class UNavigationSystemV1;
class APlayerCharacter;
class UPawnSensingComponent;
/**
 * 
 */
UCLASS()
class AGP_API ABaseAICharacter : public ABaseCharacter
{
	GENERATED_BODY()
	
public:
	ABaseAICharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
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
	TWeakObjectPtr<ABaseCharacter> SensedCharacter = nullptr;

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
	 * Some arbitrary error value for determining how close is close enough before moving onto the next step in the path.
	 */
	UPROPERTY(EditAnywhere)
	float PathfindingError = 150.0f; // 150 cm from target by default.
	FVector TargetLocation; // Targeted location for the enemy to move towards
	void SetRandomReachableLocationInRadius(const FVector& CenterPoint, const float Radius); // Sets TargetLocation to a random location around a given point that the enemy can reach
	FVector GetNormalizedEvadeTarget() const; // Used to find the direction away from the player when the enemy is evading
	float DespawnTimer = 5.0f;
	UPROPERTY() AAIController* AIController; // The AIController class allows the enemy to move towards given locations

	bool PlayerSeen();
	bool EnemySeen();
	
public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	FVector LastSeenPlayerLocation; // The location where the enemy last saw the player
	void DelayedDespawn();
	void Despawn();

private:
	UPROPERTY() UNavigationSystemV1* NavigationSystem; // The navigation system used by the enemy to determine where it can move
};
