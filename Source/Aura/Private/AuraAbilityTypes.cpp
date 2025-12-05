#include "AuraAbilityTypes.h"

UScriptStruct* FAuraGameplayEffectContext::GetScriptStruct() const
{
	return StaticStruct();
}

bool FAuraGameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	uint32 RepBits = 0;

	if (Ar.IsSaving())
	{
		if (bReplicateInstigator && Instigator.IsValid()) RepBits |= 1 << 0; // means: Repbits = Repbits bitwise OR 1 left shift 0 => last digit is a 1 and shifted no places => 31 0s and one 1 at the end
		if (bReplicateEffectCauser && EffectCauser.IsValid()) RepBits |= 1 << 1; // => everything is moved one to the left and a 1 is added on the right
		if (AbilityCDO.IsValid()) RepBits |= 1 << 2;
		if (bReplicateSourceObject && SourceObject.IsValid()) RepBits |= 1 << 3;
		if (Actors.Num() > 0) RepBits |= 1 << 4;
		if (HitResult.IsValid()) RepBits |= 1 << 5;
		if (bHasWorldOrigin) RepBits |= 1 << 6;
		if (this->bIsCriticalHit) RepBits |= 1 << 7; 
		if (this->bIsBlockedHit) RepBits |= 1 << 8;
		if (this->bIsSuccessfulDebuff) RepBits |= 1 << 9;
		if (this->DebuffDamage > 0.f) RepBits |= 1 << 10;
		if (this->DebuffDuration > 0.f) RepBits |= 1 << 11;
		if (this->DebuffFrequency > 0.f) RepBits |= 1 << 12;
		if (this->DamageType.IsValid()) RepBits |= 1 << 13;
		if (!this->DeathImpulse.IsZero()) RepBits |= 1 << 14;
		if (!this->KnockbackForce.IsZero()) RepBits |= 1 << 15;

		if (this->bIsRadialDamage)
		{
			RepBits |= 1 << 16;
			if(this->RadialDamageInnerRadius > 0.f) RepBits |= 1 << 17;
			if(this->RadialDamageOuterRadius > 0.f) RepBits |= 1 << 18;
			if(!this->RadialDamageOrigin.IsZero()) RepBits |= 1 << 19;
		}
	}

	Ar.SerializeBits(&RepBits, 15);

	if (RepBits & (1 << 0))	Ar << Instigator; // other way round with bitwise AND => only if both bits are 1, the result is 1
	if (RepBits & (1 << 1)) Ar << EffectCauser;
	if (RepBits & (1 << 2))	Ar << AbilityCDO;
	if (RepBits & (1 << 3))	Ar << SourceObject;
	if (RepBits & (1 << 4))	SafeNetSerializeTArray_Default<31>(Ar, Actors);
	if (RepBits & (1 << 5))
	{
		if (Ar.IsLoading())
		{
			if (!HitResult.IsValid())
			{
				HitResult = TSharedPtr<FHitResult>(new FHitResult());
			}
		}
		HitResult->NetSerialize(Ar, Map, bOutSuccess);
	}
	if (RepBits & (1 << 6))
	{
		Ar << WorldOrigin;
		bHasWorldOrigin = true;
	}
	else
	{
		bHasWorldOrigin = false;
	}
	if (RepBits & (1 << 7))	Ar << this->bIsCriticalHit;
	if (RepBits & (1 << 8))	Ar << this->bIsBlockedHit;
	if (RepBits & (1 << 9))	Ar << this->bIsSuccessfulDebuff;
	if (RepBits & (1 << 10)) Ar << this->DebuffDamage;
	if (RepBits & (1 << 11)) Ar << this->DebuffDuration;
	if (RepBits & (1 << 12)) Ar << this->DebuffFrequency;
	if (RepBits & (1 << 13))
	{
		if (Ar.IsLoading())
		{
			if (!this->DamageType.IsValid())
			{
				this->DamageType = TSharedPtr<FGameplayTag>(new FGameplayTag());
			}
		}
		this->DamageType->NetSerialize(Ar, Map, bOutSuccess);
	}
	if (RepBits & (1 << 14)) this->DeathImpulse.NetSerialize(Ar, Map, bOutSuccess); // Vectors can be seralized easily
	if (RepBits & (1 << 15)) this->KnockbackForce.NetSerialize(Ar, Map, bOutSuccess);
	if (RepBits & (1 << 16))
	{
		Ar << this->bIsRadialDamage;
		if (RepBits & (1 << 17)) Ar << this->RadialDamageInnerRadius;
		if (RepBits & (1 << 18)) Ar << this->RadialDamageOuterRadius;
		if (RepBits & (1 << 19)) this->RadialDamageOrigin.NetSerialize(Ar, Map, bOutSuccess);
	}

	if (Ar.IsLoading())
	{
		AddInstigator(Instigator.Get(), EffectCauser.Get()); // Just to initialize InstigatorAbilitySystemComponent
	}

	bOutSuccess = true;
	return true;
}

FAuraGameplayEffectContext* FAuraGameplayEffectContext::Duplicate() const
{
	FAuraGameplayEffectContext* NewContext = new FAuraGameplayEffectContext();
	*NewContext = *this;
	if (GetHitResult())
	{
		// Does a deep copy of the hit result
		NewContext->AddHitResult(*GetHitResult(), true);
	}
	return NewContext;
}

bool FAuraGameplayEffectContext::IsCriticalHit() const
{
	return this->bIsCriticalHit;
}

bool FAuraGameplayEffectContext::IsBlockedHit() const
{
	return this->bIsBlockedHit;
}

void FAuraGameplayEffectContext::SetIsCriticalHit(bool bIsCriticalHitIn)
{
	this->bIsCriticalHit = bIsCriticalHitIn;
}

void FAuraGameplayEffectContext::SetIsBlockedHit(bool bIsBlockedHitIn)
{
	this->bIsBlockedHit = bIsBlockedHitIn;
}

bool FAuraGameplayEffectContext::IsSuccessfulDebuff() const
{
	return this->bIsSuccessfulDebuff;
}

float FAuraGameplayEffectContext::GetDebuffDamage() const
{
	return this->DebuffDamage;
}

float FAuraGameplayEffectContext::GetDebuffDuration() const
{
	return this->DebuffDuration;
}

float FAuraGameplayEffectContext::GetDebuffFrequency() const
{
	return this->DebuffFrequency;
}

TSharedPtr<FGameplayTag> FAuraGameplayEffectContext::GetDamageType() const
{
	return this->DamageType;
}

void FAuraGameplayEffectContext::SetIsSuccessfulDebuff(bool bSuccessful)
{
	this->bIsSuccessfulDebuff = bSuccessful;
}

void FAuraGameplayEffectContext::SetDebuffDamage(float Damage)
{
	this->DebuffDamage = Damage;
}

void FAuraGameplayEffectContext::SetDebuffDuration(float Duration)
{
	this->DebuffDuration = Duration;
}

void FAuraGameplayEffectContext::SetDebuffFrequency(float Frequency)
{
	this->DebuffFrequency = Frequency;
}

void FAuraGameplayEffectContext::SetDamageType(TSharedPtr<FGameplayTag> InDamageType)
{
	this->DamageType = InDamageType;
}

FVector FAuraGameplayEffectContext::GetDeathImpulse() const
{
	return this->DeathImpulse;
}

void FAuraGameplayEffectContext::SetDeathImpulse(const FVector& Impulse)
{
	this->DeathImpulse = Impulse;
}

FVector FAuraGameplayEffectContext::GetKnockbackForce() const
{
	return this->KnockbackForce;
}

void FAuraGameplayEffectContext::SetKnockbackForce(const FVector& Force)
{
	this->KnockbackForce = Force;
}

bool FAuraGameplayEffectContext::IsRadialDamage() const
{
	return this->bIsRadialDamage;
}

void FAuraGameplayEffectContext::SetIsRadialDamage(const bool bIsRadialDamageIn)
{
	this->bIsRadialDamage = bIsRadialDamageIn;
}

float FAuraGameplayEffectContext::GetRadialDamageInnerRadius() const
{
	return this->RadialDamageInnerRadius;
}

void FAuraGameplayEffectContext::SetRadialDamageInnerRadius(const float Radius)
{
	this->RadialDamageInnerRadius = Radius;
}

float FAuraGameplayEffectContext::GetRadialDamageOuterRadius() const
{
	return this->RadialDamageOuterRadius;
}

void FAuraGameplayEffectContext::SetRadialDamageOuterRadius(const float Radius)
{
	this->RadialDamageOuterRadius = Radius;
}

FVector FAuraGameplayEffectContext::GetRadialDamageOrigin() const
{
	return this->RadialDamageOrigin;
}

void FAuraGameplayEffectContext::SetRadialDamageOrigin(const FVector Origin)
{
	this->RadialDamageOrigin = Origin;
}
