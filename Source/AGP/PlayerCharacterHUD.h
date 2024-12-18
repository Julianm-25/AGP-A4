// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "PlayerCharacterHUD.generated.h"

/**
 * 
 */
UCLASS()
class AGP_API UPlayerCharacterHUD : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, meta=(BindWidget)) UProgressBar* HealthBar;
	UPROPERTY(EditAnywhere, meta=(BindWidget)) UTextBlock* AmmoText;
	UPROPERTY(EditAnywhere, meta=(BindWidget)) UTextBlock* WaveText;
	UPROPERTY(EditAnywhere, meta=(BindWidget)) UTextBlock* EnemiesLeftText;
	UPROPERTY(Transient, meta=(BindWidgetAnim)) UWidgetAnimation* DamageAnim;

public:
	void SetHealthBar(float HealthPercent);
	void SetAmmoText(int32 RoundsRemaining, int32 MagazineSize);
	void SetWaveText(int32 Wave);
	void SetEnemiesLeftText(int32 EnemiesLeft);
	void PlayDamageAnimation();
};
