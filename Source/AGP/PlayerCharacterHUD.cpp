// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacterHUD.h"

void UPlayerCharacterHUD::SetHealthBar(float HealthPercent) // Sets the visual of the health bar to the given value
{
	if (HealthBar) HealthBar->SetPercent(HealthPercent);
}

void UPlayerCharacterHUD::SetAmmoText(int32 RoundsRemaining, int32 MagazineSize) // Sets the ammo text to the given values
{
	if (AmmoText) AmmoText->SetText(FText::FromString(FString::FromInt(RoundsRemaining) + "/" + FString::FromInt(MagazineSize)));
}

void UPlayerCharacterHUD::SetWaveText(int32 Wave) // Sets the wave text to the given value
{
	if (WaveText) WaveText->SetText(FText::FromString("Wave " + FString::FromInt(Wave)));
}

void UPlayerCharacterHUD::SetEnemiesLeftText(int32 EnemiesLeft) // Sets the enemies left text to the given value
{
	if (EnemiesLeftText) EnemiesLeftText->SetText(FText::FromString("Enemies left: " + FString::FromInt(EnemiesLeft)));
}

void UPlayerCharacterHUD::PlayDamageAnimation() // Plays the damage flash animation on the HUD
{
	PlayAnimation(DamageAnim);
}

void UPlayerCharacterHUD::ShowGameOverText() // Reveals the text in the Game Over box and makes all other HUD elements completely transparent
{
	GameOverBox->SetRenderOpacity(1.0f);
	EnemiesLeftText->SetRenderOpacity(0.0f);
	WaveText->SetRenderOpacity(0.0f);
	AmmoText->SetRenderOpacity(0.0f);
	HealthBar->SetRenderOpacity(0.0f);
}

void UPlayerCharacterHUD::SetWaveReachedText(int32 Wave) // Sets the wave reached text to the given value
{
	if (WaveReachedText) WaveReachedText->SetText(FText::FromString("You reached wave " + FString::FromInt(Wave)));
}
