// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponComponent.h"

#include "BaseCharacter.h"
#include "HealthComponent.h"
#include "AGP/AGPGameInstance.h"
#include "NiagaraComponent.h"
#include "PlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UWeaponComponent::UWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	// ...
}

void UWeaponComponent::Fire(const FVector& BulletStart, const FVector& FireAtLocation)
{
	ServerFire(BulletStart, FireAtLocation);
}

void UWeaponComponent::Reload()
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		ReloadImplementation();
	}
	else
	{
		ServerReload();
	}
}

void UWeaponComponent::CompleteReload()
{
	UE_LOG(LogTemp, Display, TEXT("Reload Complete"))
	RoundsRemainingInMagazine = WeaponStats.MagazineSize;
	UpdateAmmoUI();
}

bool UWeaponComponent::FireImplementation(const FVector& BulletStart, const FVector& FireAtLocation,
	FVector& OutHitLocation, FString& OutHitActor, FVector& OutFireDirection)
{
	// Determine if the weapon is able to fire.
	if (TimeSinceLastShot < WeaponStats.FireRate || IsMagazineEmpty() || bIsReloading)
	{
		return false;
	}

	// In order to integrate the weapon accuracy, we will need some logic to shift the FireAtLocation.
	// The general rule for the accuracy stat is:
	// An accuracy of 1.0f will not change the FireAtLocation and it will hit directly where they are aiming.
	// An accuracy of 0.0f will fire in some random direction completely disregarding the FireAtLocation.
	// The closer to 1.0f, the closer the shot will land to their fire at location.
	
	// Creates a random direction vector.
	FVector RandomFireAt = FMath::VRand();
	float CurrentShotDistance = FVector::Distance(BulletStart, FireAtLocation);
	// Makes that random direction vector the same length as the current shot between the bullet start and fire at location.
	RandomFireAt *= CurrentShotDistance;
	// Offsets the direction vector by the Bullet Start position making this RandomFireAt now contain a random position
	// somewhere on the surface a sphere surrounding the bullet start position. The original FireAtLocation is also on
	// the surface of this same sphere.
	RandomFireAt += BulletStart;
	// Now we just need to blend between these two positions based on the accuracy value.
	FVector AccuracyAdjustedFireAt = FMath::Lerp(RandomFireAt, FireAtLocation, WeaponStats.Accuracy);
	OutFireDirection = AccuracyAdjustedFireAt;
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());
	if (GetWorld()->LineTraceSingleByChannel(HitResult, BulletStart, AccuracyAdjustedFireAt, ECC_WorldStatic, QueryParams))
	{
		OutHitLocation = HitResult.ImpactPoint;
		if (ABaseCharacter* HitCharacter = Cast<ABaseCharacter>(HitResult.GetActor()))
		{
			if (AEnemyCharacter* HitEnemy = Cast<AEnemyCharacter>(HitResult.GetActor())) // Only dealing damage when enemies are shot to remove friendly fire
			{
				if (UHealthComponent* HitCharacterHealth = HitEnemy->GetComponentByClass<UHealthComponent>())
				{
					HitCharacterHealth->ApplyDamage(WeaponStats.BaseDamage);
				}
			}
			OutHitActor = "Character"; // Regardless of whether the shot hits a player or enemy, the blood particle effect will still play on hit
		}
		else
		{
			OutHitActor = "Terrain"; // Any other collision is considered terrain
		}
	}
	else
	{
		OutHitLocation = AccuracyAdjustedFireAt;
		OutHitActor = "Void"; // If it hits nothing, it is "Void"
	}
	
	TimeSinceLastShot = 0.0f;
	RoundsRemainingInMagazine--;
	UpdateAmmoUI();
	return true;
}

void UWeaponComponent::FireVisualImplementation(const FVector& BulletStart, const FVector& HitLocation, const FString& HitActor, const FVector& FireDirection)
{
	if(APawn* Owner = Cast<APawn>(GetOwner()))
	{
		if (Owner->IsLocallyControlled())
		{
			if (UAGPGameInstance* GameInstance = Cast<UAGPGameInstance>(GetWorld()->GetGameInstance()))
			{
				GameInstance->PlayGunshotSound2D(); // Play a directionless sound for the player who shot
			}
			// Get the muzzle flash component on the player and activate it
			if (UNiagaraComponent* MuzzleFlashComponent = Cast<UNiagaraComponent>(Owner->GetDefaultSubobjectByName(TEXT("MuzzleFlash")))) 
			{
				MuzzleFlashComponent->DeactivateImmediate(); // Deactivating it before activating it so that it can activate multiple times in rapid succession
				MuzzleFlashComponent->Activate();
				BulletShotVisual(MuzzleFlashComponent->GetComponentLocation(), FireDirection); // Spawn the bullet particle system from the muzzle in the direction of the shot 
			}
		}
		else
		{
			if (UAGPGameInstance* GameInstance = Cast<UAGPGameInstance>(GetWorld()->GetGameInstance()))
			{
				GameInstance->PlayGunshotSoundAtLocation(BulletStart); // Play a directional sound for all other players
			}
			// The muzzle is in a different position on the full body, so we need to activate the particles differently for other players
			if (UNiagaraComponent* FullBodyMuzzleFlashComponent = Cast<UNiagaraComponent>(Owner->GetDefaultSubobjectByName(TEXT("FullBodyMuzzleFlash")))) 
			{
				FullBodyMuzzleFlashComponent->DeactivateImmediate();
				FullBodyMuzzleFlashComponent->Activate();
				BulletShotVisual(FullBodyMuzzleFlashComponent->GetComponentLocation(), FireDirection);
			}
		}
	}

	BulletHitVisual(HitActor, HitLocation); // Play the bullet hit particles if the bullet hit something
	
	if (ABaseCharacter* Owner = Cast<ABaseCharacter>(GetOwner()))
	{
		Owner->FireWeaponGraphical(); // Play the firing animation
	}
}

// Spawns particles for where the bullet hits
void UWeaponComponent::BulletHitVisual(FString HitActor, FVector HitLocation)
{
	if (UAGPGameInstance* GameInstance = Cast<UAGPGameInstance>(GetWorld()->GetGameInstance()))
	{
		if (HitActor == "Character")
		{
			GameInstance->SpawnCharacterHitParticles(HitLocation); // If it hits a character spawn character hit particles
		}
		else if (HitActor == "Terrain")
		{
			GameInstance->SpawnTerrainHitParticles(HitLocation); // If it hits terrain spawn terrain hit particles
		}
	}
}

// Spawns particles to show the bullet and its path
void UWeaponComponent::BulletShotVisual(FVector BulletStart, FVector FireAtLocation)
{
	if (UAGPGameInstance* GameInstance = Cast<UAGPGameInstance>(GetWorld()->GetGameInstance()))
	{
		FVector BulletVector = FireAtLocation - BulletStart;
		UKismetMathLibrary::Normal(BulletVector); // Normalise the difference between the start of the bullet and its destination to get the direction
		GameInstance->SpawnBulletParticles(BulletStart, BulletVector.Rotation()); // Use the direction to tell the bullet particles where to go
	}
}

void UWeaponComponent::ReloadImplementation()
{
	// Shouldn't be able to reload if you are already reloading.
	if (bIsReloading) return;

	UE_LOG(LogTemp, Display, TEXT("Start Reload"))
	bIsReloading = true;
}

void UWeaponComponent::ServerReload_Implementation()
{
	ReloadImplementation();
}

void UWeaponComponent::ServerFire_Implementation(const FVector& BulletStart, const FVector& FireAtLocation)
{
	FVector HitLocation;
	FString HitActor = "nullptr";
	FVector FireDirection;
	if(FireImplementation(BulletStart, FireAtLocation, HitLocation, HitActor, FireDirection))
	{
		MulticastFire(BulletStart, HitLocation, HitActor, FireDirection);
	}
}

void UWeaponComponent::MulticastFire_Implementation(const FVector& BulletStart, const FVector& HitLocation, const FString& HitActor, const FVector& FireDirection)
{
	//UE_LOG(LogTemp, Display, TEXT("MULTICAST FIRING"));
	FireVisualImplementation(BulletStart, HitLocation, HitActor, FireDirection);
}

void UWeaponComponent::SetWeaponStats(const FWeaponStats& WeaponInfo, EWeaponRarity Rarity)
{
	this->WeaponRarity = Rarity;
	this->WeaponStats = WeaponInfo;
	// Set the number of bullets to the magazine size
	RoundsRemainingInMagazine = WeaponInfo.MagazineSize;
	UpdateAmmoUI();
}

FWeaponStats UWeaponComponent::GetWeaponStats()
{
	return this->WeaponStats;
}

EWeaponRarity UWeaponComponent::GetWeaponRarity()
{
	return this->WeaponRarity;
}

bool UWeaponComponent::IsMagazineEmpty()
{
	return RoundsRemainingInMagazine <= 0;
}

void UWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UWeaponComponent, RoundsRemainingInMagazine); // Replicating ammo left in the magazine
	DOREPLIFETIME(UWeaponComponent, WeaponStats); // and the weapon stats
}

// Called when the game starts
void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


void UWeaponComponent::UpdateAmmoUI()
{
	if(APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwner()))
	{
		PlayerCharacter->UpdateAmmoUI(RoundsRemainingInMagazine, WeaponStats.MagazineSize);
	}
}

// Called every frame
void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	TimeSinceLastShot += DeltaTime;

	// Logic that delays the call to CompleteReload if the weapon is currently being reloaded.
	if (bIsReloading)
	{
		CurrentReloadDuration += DeltaTime;
		if (CurrentReloadDuration >= WeaponStats.ReloadTime)
		{
			bIsReloading = false;
			CompleteReload();
			CurrentReloadDuration = 0.0f;
		}
	}
}

