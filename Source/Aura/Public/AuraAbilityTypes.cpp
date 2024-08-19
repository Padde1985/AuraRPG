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
	}

	Ar.SerializeBits(&RepBits, 9);

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
