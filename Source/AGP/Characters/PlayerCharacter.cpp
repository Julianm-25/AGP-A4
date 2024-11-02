// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "EngineUtils.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AGP/AGPGameInstance.h"
#include "AGP/Pickups/WeaponPickup.h"
#include "Kismet/KismetMathLibrary.h"

class AWeaponPickup;
// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}

	if (IsLocallyControlled() && PlayerHUDClass)
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
		{
			PlayerHUD = CreateWidget<UPlayerCharacterHUD>(PlayerController, PlayerHUDClass);
			if (PlayerHUD)
			{
				PlayerHUD->AddToPlayerScreen();
			}
		}
	}
	UpdateHealthBar(1.0f);

	for (TActorIterator<AWeaponPickup> It(GetWorld()); It; ++It)
	{
		It->MulticastUpdateVisual();
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
		Input->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		Input->BindAction(FireAction, ETriggerEvent::Triggered, this, &APlayerCharacter::FireWeapon);
		Input->BindAction(ReloadAction, ETriggerEvent::Started, this, &ABaseCharacter::Reload);
	}
}

void APlayerCharacter::UpdateHealthBar(float HealthPercent)
{
	if (IsLocallyControlled() && PlayerHUD)
	{
		PlayerHUD->SetHealthBar(HealthPercent);
	}
}

void APlayerCharacter::UpdateAmmoUI(int32 RoundsRemaining, int32 MagazineSize)
{
	if (IsLocallyControlled() && PlayerHUD)
	{
		PlayerHUD->SetAmmoText(RoundsRemaining, MagazineSize);
	}
}

void APlayerCharacter::UpdateWaveCount(int32 Wave)
{
	if (IsLocallyControlled() && PlayerHUD)
	{
		PlayerHUD->SetWaveText(Wave);
		PlayerHUD->SetWaveReachedText(Wave);
	}
}

void APlayerCharacter::UpdateEnemiesLeftCount(int32 EnemiesLeft)
{
	if (IsLocallyControlled() && PlayerHUD)
	{
		PlayerHUD->SetEnemiesLeftText(EnemiesLeft);
	}
}

// Plays the HUD damage flash
void APlayerCharacter::PlayDamageAnimation()
{
	if (IsLocallyControlled() && PlayerHUD)
	{
		PlayerHUD->PlayDamageAnimation();
	}
}

// Update wave and enemies on all clients
void APlayerCharacter::MulticastUpdateWaveAndEnemies_Implementation(int32 Wave, int32 EnemiesLeft)
{
	UpdateWaveCount(Wave);
	UpdateEnemiesLeftCount(EnemiesLeft);
}

// Play damage animation, spawn hit particles and play hit sound across all clients
void APlayerCharacter::MulticastTakeDamage_Implementation()
{
	PlayDamageAnimation();
	if (UAGPGameInstance* GameInstance = Cast<UAGPGameInstance>(GetWorld()->GetGameInstance()))
	{
		GameInstance->SpawnCharacterHitParticles(GetActorLocation());
		GameInstance->PlayMeleeHitSoundAtLocation(GetActorLocation());
	}
}

// Destroy the player on all clients
void APlayerCharacter::MulticastKillPlayer_Implementation()
{
	if (IsLocallyControlled() && PlayerHUD)
	{
		PlayerHUD->ShowGameOverText(); // Reveals the Game Over text on the HUD of the killed player
	}
	Destroy();
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	const FVector ForwardVector = GetActorForwardVector();
	AddMovementInput(ForwardVector, MovementVector.X);
	const FVector RightVector = GetActorRightVector();
	AddMovementInput(RightVector, MovementVector.Y);
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookVector = Value.Get<FVector2D>() * LookSensitivity;
	AddControllerYawInput(LookVector.X);
	AddControllerPitchInput(LookVector.Y);
}

void APlayerCharacter::FireWeapon(const FInputActionValue& Value)
{
	FVector CameraPosition;
	FRotator CameraRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(CameraPosition, CameraRotation);
	const FVector CameraForward = UKismetMathLibrary::GetForwardVector(CameraRotation);
	if (BulletStartPosition)
	{
		Fire(BulletStartPosition->GetComponentLocation() + 10000.0f * CameraForward);
	}
}

