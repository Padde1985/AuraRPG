#include "AbilitySystem/Abilities/AuraFireBolt.h"
#include "AuraGameplayTags.h"
#include "Interaction/CombatInterface.h"
#include "AbilitySystem/AuraAbilitysystemLibrary.h"
#include "Actor/AuraProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "Gameframework/ProjectileMovementComponent.h"

FString UAuraFireBolt::GetDescription(int32 Level)
{
	// get scaled damage from curve table for a given level
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);

	if (Level == 1)
	{
		return FString::Printf(TEXT("<Title>FIRE BOLT</>\n\n"
										"<Default>Launches %d bolt of fire, exploding on impact and dealing </><Damage>%d </>"
										"<Default>fire damage with a chance to burn</>\n\n"
										"<Small>Level: </><Level>%d</>\n"
										"<Small>ManaCost: </><ManaCost>%.0f</>\n"
										"<Small>Cooldown: </><Cooldown>%.1f</>"),
									Level, ScaledDamage, Level, ManaCost, Cooldown);
	}
	else
	{
		return FString::Printf(TEXT("<Title>FIRE BOLT</>\n\n"
										"<Default>Launches %d bolts of fire, exploding on impact and dealing </><Damage>%d </>"
										"<Default>fire damage with a chance to burn</>\n\n"
										"<Small>Level: </><Level>%d</>\n"
										"<Small>ManaCost: </><ManaCost>%.0f</>\n"
										"<Small>Cooldown: </><Cooldown>%.1f</>"),
									FMath::Min(Level, this->MaxProjectiles), ScaledDamage, Level, ManaCost, Cooldown);
	}
}

FString UAuraFireBolt::GetNextLevelDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);

	return FString::Printf(TEXT("<Title>Next Level</>\n\n"
									"<Default>Launches %d bolts of fire, exploding on impact and dealing </><Damage>%d </>"
									"<Default>fire damage with a chance to burn</>\n\n"
									"<Small>Level: </><Level>%d</>\n"
									"<Small>ManaCost: </><ManaCost>%.0f</>\n"
									"<Small>Cooldown: </><Cooldown>%.1f</>"),
								FMath::Min(Level, this->MaxProjectiles), ScaledDamage, Level, ManaCost, Cooldown);
}

void UAuraFireBolt::SpawnProjectiles(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, AActor* HomingTarget, bool bPitchOverride, float Pitch)
{
	const int32 NumProjectiles = FMath::Min(this->MaxProjectiles, GetAbilityLevel());
	if (NumProjectiles > 1)
	{
		bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
		if (!bIsServer) return;

		const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), SocketTag);
		FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();

		if (bPitchOverride) Rotation.Pitch = Pitch;

		const FVector Forward = Rotation.Vector();
		TArray<FVector> Directions = UAuraAbilitysystemLibrary::EvenlyRotatedVectors(Forward, FVector::UpVector, this->ProjectileSpread, NumProjectiles);
		for (const FVector& Direction : Directions)
		{
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(SocketLocation);
			SpawnTransform.SetRotation(Direction.Rotation().Quaternion());

			AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(ProjectileClass, SpawnTransform, GetAvatarActorFromActorInfo(),
																						  Cast<APawn>(GetAvatarActorFromActorInfo()), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

			Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();

			if (HomingTarget && HomingTarget->Implements<UCombatInterface>())
			{
				Projectile->ProjectileMovement->HomingTargetComponent = HomingTarget->GetRootComponent();
			}
			else
			{
				// trick 17&4: make the Homing Target Component garbage collected using a middle man that is garbage collected
				Projectile->HomingTargetSceneComponent = NewObject<USceneComponent>(USceneComponent::StaticClass());
				Projectile->HomingTargetSceneComponent->SetWorldLocation(ProjectileTargetLocation);
				Projectile->ProjectileMovement->HomingTargetComponent = Projectile->HomingTargetSceneComponent;
			}
			Projectile->ProjectileMovement->HomingAccelerationMagnitude = FMath::FRandRange(this->HomingAccelerationMin, this->HomingAccelerationMax);
			Projectile->ProjectileMovement->bIsHomingProjectile = this->bLaunchHomingProjectiles;

			Projectile->FinishSpawning(SpawnTransform); //has to be called due to using ActorDeferred
		}
	}
	else
	{
		SpawnProjectile(ProjectileTargetLocation, SocketTag, bPitchOverride, Pitch);
	}
}
