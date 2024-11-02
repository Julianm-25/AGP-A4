// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacterHUD.h"

void UPlayerCharacterHUD::SetHealthBar(float HealthPercent)
{
	if (HealthBar) HealthBar->SetPercent(HealthPercent);
}

void UPlayerCharacterHUD::SetAmmoText(int32 RoundsRemaining, int32 MagazineSize)
{
	if (AmmoText) AmmoText->SetText(FText::FromString(FString::FromInt(RoundsRemaining) + "/" + FString::FromInt(MagazineSize)));
}

void UPlayerCharacterHUD::SetWaveText(int32 Wave)
{
	if (WaveText) WaveText->SetText(FText::FromString("Wave " + FString::FromInt(Wave)));
}

void UPlayerCharacterHUD::SetEnemiesLeftText(int32 EnemiesLeft)
{
	if (EnemiesLeftText) EnemiesLeftText->SetText(FText::FromString("Enemies left: " + FString::FromInt(EnemiesLeft)));
}

void UPlayerCharacterHUD::PlayDamageAnimation()
{
	PlayAnimation(DamageAnim);
}

void UPlayerCharacterHUD::ShowGameOverText()
{
	GameOverBox->SetRenderOpacity(1.0f);
	EnemiesLeftText->SetRenderOpacity(0.0f);
	WaveText->SetRenderOpacity(0.0f);
	AmmoText->SetRenderOpacity(0.0f);
	HealthBar->SetRenderOpacity(0.0f);
}

void UPlayerCharacterHUD::SetWaveReachedText(int32 Wave)
{
	if (WaveReachedText) WaveReachedText->SetText(FText::FromString("You reached wave " + FString::FromInt(Wave)));
}
