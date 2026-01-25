#include "AbilitySystem/Abilities/AuraFireBlast.h"
#include "AbilitySystem/AuraAbilitysystemLibrary.h"
#include "Actor/AuraFireBall.h"

// get description for current level
FString UAuraFireBlast::GetDescription(int32 Level)
{
	// get scaled damage from curve table for a given level
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);

	return FString::Printf(TEXT("<Title>FIRE BLAST</>\n\n"
							"<Default>Launches %d Fire balls in all directions. Each coming back</>"
							"<Default>and exploding upon return, causing </><Damage>%d </>"
							"<Default>radial fire damage with the chance to burn</>\n\n"
							"<Small>Level: </><Level>%d</>\n"
							"<Small>ManaCost: </><ManaCost>%.0f</>\n"
							"<Small>Cooldown: </><Cooldown>%.1f</>"),
							this->NumFireBalls, ScaledDamage, Level, ManaCost, Cooldown);
}

// get description for next level (description does not change over levels)
FString UAuraFireBlast::GetNextLevelDescription(int32 Level)
{
	return this->GetDescription(Level);
}

// spawn multiple fireballs depending on ability level
TArray<AAuraFireBall*> UAuraFireBlast::SpawnFireBalls()
{
	if (!GetAvatarActorFromActorInfo()->HasAuthority()) return TArray<AAuraFireBall*>();

	TArray<AAuraFireBall*> FireBalls;
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	TArray<FRotator> Rotators = UAuraAbilitysystemLibrary::EvenlySpacedRotators(Forward, FVector::UpVector, 360.f, this->NumFireBalls);
	for (const FRotator& Rotator : Rotators)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(Location);
		SpawnTransform.SetRotation(Rotator.Quaternion());
		AAuraFireBall* FireBall = GetWorld()->SpawnActorDeferred<AAuraFireBall>(this->FireBallClass, SpawnTransform, GetOwningActorFromActorInfo(), 
																				CurrentActorInfo->PlayerController->GetPawn(), 
																				ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		FireBall->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();
		FireBall->SetOwner(GetAvatarActorFromActorInfo());
		FireBall->ExplosionDamageParams = MakeDamageEffectParamsFromClassDefaults();

		FireBalls.Add(FireBall);

		FireBall->FinishSpawning(SpawnTransform);
	}

	return FireBalls;
}