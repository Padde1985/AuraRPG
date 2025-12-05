#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(this->DamageEffectClass, 1.f);
	const float DamageMagnitude = this->Damage.GetValueAtLevel(GetAbilityLevel());
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, this->DamageType, DamageMagnitude);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
}

FDamageEffectParams UAuraDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor, FVector InRadialDamageOrigin) const
{
	FDamageEffectParams Params;
	Params.WorldContextObject = GetAvatarActorFromActorInfo();
	Params.DamageGameplayEffectClass = this->DamageEffectClass;
	Params.SourceAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	Params.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	Params.BaseDamage = this->Damage.GetValueAtLevel(GetAbilityLevel());
	Params.AbilityLevel = GetAbilityLevel();
	Params.DamageType = this->DamageType;
	Params.DebuffChance = this->DebuffChance;
	Params.DebuffDamage = this->DebuffDamage;
	Params.DebuffDuration = this->DebuffDuration;
	Params.DebuffFrequency = this->DebuffFrequency;
	Params.DeathImpulseMagnitude = this->DeathImpulseMagnitude;
	Params.KnockbackForceMagnitude = this->KnockbackForceMagnitude;
	Params.KnockbackChance = this->KnockbackChance;
	if (IsValid(TargetActor))
	{
		FRotator Rotation = FRotator::ZeroRotator;
		if (this->bIsRadialDamage)
		{
			Params.bIsRadialDamage = this->bIsRadialDamage;
			Params.RadialDamageOrigin = InRadialDamageOrigin;
			Params.RadialDamageInnerRadius = this->RadialDamageInnerRadius;
			Params.RadialDamageOuterRadius = this->RadialDamageOuterRadius;

			Rotation = (TargetActor->GetActorLocation() - Params.RadialDamageOrigin).Rotation();
		}
		else
		{
			Rotation = (TargetActor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).Rotation();
		}
		Rotation.Pitch = 45.f;
		
		const FVector ToTarget = Rotation.Vector();
		Params.DeathImpulse = ToTarget * this->DeathImpulseMagnitude;

		const bool bKnockback = FMath::RandRange(1, 100) < Params.KnockbackChance;
		if (bKnockback)	Params.KnockbackForce = ToTarget * this->KnockbackForceMagnitude;
	}

	return Params;
}

float UAuraDamageGameplayAbility::GetDamageAtLevel() const
{
	return this->Damage.GetValueAtLevel(GetAbilityLevel());
}
