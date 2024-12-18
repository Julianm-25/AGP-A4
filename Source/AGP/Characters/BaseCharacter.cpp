// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"

#include "EnemyCharacter.h"
#include "HealthComponent.h"
#include "AGP/AGPGameInstance.h"
#include "AGP/WaveSpawnSubsystem.h"
#include "AGP/Pickups/WeaponPickup.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BulletStartPosition = CreateDefaultSubobject<USceneComponent>("Bullet Start");
	BulletStartPosition->SetupAttachment(GetRootComponent());
	HealthComponent = CreateDefaultSubobject<UHealthComponent>("Health Component");
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseCharacter::Fire(const FVector& FireAtLocation)
{
	if (HasWeapon())
	{
		WeaponComponent->Fire(BulletStartPosition->GetComponentLocation(), FireAtLocation);
	}
}

void ABaseCharacter::Reload()
{
	if (HasWeapon())
	{
		WeaponComponent->Reload();
	}
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABaseCharacter, WeaponComponent);
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ABaseCharacter::HasWeapon()
{
	return (WeaponComponent != nullptr);
}

void ABaseCharacter::EquipWeapon(bool bEquipWeapon, EWeaponRarity Rarity, const FWeaponStats& WeaponStats)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		EquipWeaponImplementation(bEquipWeapon, Rarity, WeaponStats);
		MulticastEquipWeapon(bEquipWeapon);
	}
}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABaseCharacter::Ragdoll()
{
	if (AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(this))
	{
		if (FMath::RandRange(1,20) == 20)
		{
			if (const UAGPGameInstance* GameInstance = GetWorld()->GetGameInstance<UAGPGameInstance>())
			{
				UE_LOG(LogTemp, Display, TEXT("WEAPON SHOULD SPAWN HERE"))
				GetWorld()->SpawnActor<AWeaponPickup>(GameInstance->GetWeaponPickupClass(), GetActorLocation(), FRotator::ZeroRotator);
			}
		}
		EnemyCharacter->DelayedDespawn();
		if (UWaveSpawnSubsystem* WaveSpawnSubsystem = GetWorld()->GetSubsystem<UWaveSpawnSubsystem>())
		{
			WaveSpawnSubsystem->DecrementEnemyCount();
		}
	}
	GetCharacterMovement()->DisableMovement();
	GetMesh()->SetSimulatePhysics(true);
	GetCapsuleComponent()->DestroyComponent();
	
}

void ABaseCharacter::EquipWeaponImplementation(bool bEquipWeapon, EWeaponRarity Rarity, const FWeaponStats& WeaponStats)
{
	// Create or remove the weapon component depending on whether we are trying to equip a weapon and we
	// don't already have one. Or if we are trying to unequip a weapon and we do have one.
	if (bEquipWeapon && !HasWeapon())
	{
		WeaponComponent = NewObject<UWeaponComponent>(this);
		WeaponComponent->RegisterComponent();
	}
	else if (!bEquipWeapon && HasWeapon())
	{
		WeaponComponent->UnregisterComponent();
		WeaponComponent = nullptr;
	}

	// At this point we should have a WeaponComponent if we are trying to equip a weapon.
	if (HasWeapon())
	{
		// Set the weapons stats to the given weapon stats.
		UE_LOG(LogTemp, Display, TEXT("Equipping weapon: \n%s"), *WeaponStats.ToString())
		WeaponComponent->SetWeaponStats(WeaponStats, Rarity);
	}
	
	EquipWeaponGraphical(bEquipWeapon);
	if (bEquipWeapon)
	{
		UE_LOG(LogTemp, Display, TEXT("Player has equipped weapon."))
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Player has unequipped weapon."))
	}
}


void ABaseCharacter::MulticastEquipWeapon_Implementation(bool bEquipWeapon)
{
	EquipWeaponGraphical(bEquipWeapon);
}

