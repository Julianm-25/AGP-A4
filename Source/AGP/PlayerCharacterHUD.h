// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "PlayerCharacterHUD.generated.h"

/**
 * 
 */
UCLASS()
class AGP_API UPlayerCharacterHUD : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, meta=(BindWidget)) UProgressBar* HealthBar;  // The player's health bar
	UPROPERTY(EditAnywhere, meta=(BindWidget)) UTextBlock* AmmoText;  // Text for the player's current and maximum ammo
	UPROPERTY(EditAnywhere, meta=(BindWidget)) UTextBlock* WaveText; // Text for the current wave
	UPROPERTY(EditAnywhere, meta=(BindWidget)) UTextBlock* EnemiesLeftText; // Text for the number of enemies left in the wave
	UPROPERTY(Transient, meta=(BindWidgetAnim)) UWidgetAnimation* DamageAnim; // Animation that plays when taking damage
	UPROPERTY(EditAnywhere, meta=(BindWidget)) UTextBlock* WaveReachedText; // Text showing the wave reached when the player is killed
	UPROPERTY(EditAnywhere, meta=(BindWidget)) UVerticalBox* GameOverBox; // Text box holding the Game Over text and the text showing the wave reached

public:
	void SetHealthBar(float HealthPercent); // Sets the visual of the health bar to the given value
	void SetAmmoText(int32 RoundsRemaining, int32 MagazineSize); // Sets the ammo text to the given values
	void SetWaveText(int32 Wave); // Sets the wave text to the given value
	void SetEnemiesLeftText(int32 EnemiesLeft); // Sets the enemies left text to the given value
	void PlayDamageAnimation(); // Plays the damage flash animation on the HUD
	void ShowGameOverText(); // Reveals the text in the Game Over box and makes all other HUD elements completely transparent
	void SetWaveReachedText(int32 Wave); // Sets the wave reached text to the given value
};
