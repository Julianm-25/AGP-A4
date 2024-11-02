// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.h"
#include "InputActionValue.h"
#include "AGP/PlayerCharacterHUD.h"
#include "PlayerCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
UCLASS()
class AGP_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* MoveAction;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* LookAction;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* JumpAction;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* FireAction;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* ReloadAction;
	UPROPERTY(EditDefaultsOnly)
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditDefaultsOnly)
	float LookSensitivity = 0.5f;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPlayerCharacterHUD> PlayerHUDClass;
	UPROPERTY()
	UPlayerCharacterHUD* PlayerHUD;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void UpdateHealthBar(float HealthPercent);
	void UpdateAmmoUI(int32 RoundsRemaining, int32 MagazineSize);
	void UpdateWaveCount(int32 Wave);
	void UpdateEnemiesLeftCount(int32 EnemiesLeft);
	void PlayDamageAnimation();
	UFUNCTION(NetMulticast, Reliable) void MulticastUpdateWaveAndEnemies(int32 Wave, int32 EnemiesLeft);
	UFUNCTION(NetMulticast, Unreliable) void MulticastTakeDamage();

	UFUNCTION(NetMulticast, Reliable) void MulticastKillPlayer();
	
private:
	
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void FireWeapon(const FInputActionValue& Value);

};
