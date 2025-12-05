#pragma once

#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"

class UGameplayEffect;
class UAbilitySystemComponent;

USTRUCT(BlueprintType)
struct FDamageEffectParams
{
	GENERATED_BODY()

	FDamageEffectParams() {};

	UPROPERTY(BlueprintReadWrite) TObjectPtr<UObject> WorldContextObject = nullptr;
	UPROPERTY(BlueprintReadWrite) TSubclassOf<UGameplayEffect> DamageGameplayEffectClass = nullptr;
	UPROPERTY(BlueprintReadWrite) TObjectPtr<UAbilitySystemComponent> SourceAbilitySystemComponent = nullptr;
	UPROPERTY(BlueprintReadWrite) TObjectPtr<UAbilitySystemComponent> TargetAbilitySystemComponent = nullptr;
	UPROPERTY(BlueprintReadWrite) float BaseDamage = 0.f;
	UPROPERTY(BlueprintReadWrite) float AbilityLevel = 1.f;
	UPROPERTY(BlueprintReadWrite) FGameplayTag DamageType = FGameplayTag();
	UPROPERTY(BlueprintReadWrite) float DebuffChance = 0.f;
	UPROPERTY(BlueprintReadWrite) float DebuffDamage = 0.f;
	UPROPERTY(BlueprintReadWrite) float DebuffDuration = 0.f;
	UPROPERTY(BlueprintReadWrite) float DebuffFrequency = 0.f;
	UPROPERTY(BlueprintReadWrite) float DeathImpulseMagnitude = 0.f;
	UPROPERTY(BlueprintReadWrite) FVector DeathImpulse = FVector::ZeroVector;
	UPROPERTY(BlueprintReadWrite) float KnockbackForceMagnitude = 0.f;
	UPROPERTY(BlueprintReadWrite) FVector KnockbackForce = FVector::ZeroVector;
	UPROPERTY(BlueprintReadWrite) float KnockbackChance = 0.f;
	UPROPERTY(BlueprintReadWrite) bool bIsRadialDamage = false;
	UPROPERTY(BlueprintReadWrite) float RadialDamageInnerRadius = 0.f;
	UPROPERTY(BlueprintReadWrite) float RadialDamageOuterRadius = 0.f;
	UPROPERTY(BlueprintReadWrite) FVector RadialDamageOrigin = FVector::ZeroVector;
};

USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

public:
	/** Returns the actual struct used for serialization, subclasses must override this! */
	virtual UScriptStruct* GetScriptStruct() const override;

	/** Custom serialization, subclasses must override this */
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;

	/** Creates a copy of this context, used to duplicate for later modifications */
	virtual FAuraGameplayEffectContext* Duplicate() const;

	bool IsCriticalHit() const;
	bool IsBlockedHit() const;
	void SetIsCriticalHit(bool bIsCriticalHitIn);
	void SetIsBlockedHit(bool bIsBlockedHitIn);
	bool IsSuccessfulDebuff() const;
	float GetDebuffDamage() const;
	float GetDebuffDuration() const;
	float GetDebuffFrequency() const;
	TSharedPtr<FGameplayTag> GetDamageType() const;
	void SetIsSuccessfulDebuff(bool bSuccessful);
	void SetDebuffDamage(float Damage);
	void SetDebuffDuration(float Duration);
	void SetDebuffFrequency(float Frequency);
	void SetDamageType(TSharedPtr<FGameplayTag> InDamageType);
	FVector GetDeathImpulse() const;
	void SetDeathImpulse(const FVector& Impulse);
	FVector GetKnockbackForce() const;
	void SetKnockbackForce(const FVector& Force);
	bool IsRadialDamage() const;
	void SetIsRadialDamage(const bool bIsRadialDamageIn);
	float GetRadialDamageInnerRadius() const;
	void SetRadialDamageInnerRadius(const float Radius);
	float GetRadialDamageOuterRadius() const;
	void SetRadialDamageOuterRadius(const float Radius);
	FVector GetRadialDamageOrigin() const;
	void SetRadialDamageOrigin(const FVector Origin);

protected:
	UPROPERTY() bool bIsCriticalHit = false;
	UPROPERTY() bool bIsBlockedHit = false;
	UPROPERTY() bool bIsSuccessfulDebuff = false;
	UPROPERTY() float DebuffDamage = 0.f;
	UPROPERTY() float DebuffDuration = 0.f;
	UPROPERTY() float DebuffFrequency = 0.f;
	UPROPERTY() FVector DeathImpulse = FVector::ZeroVector;
	UPROPERTY() FVector KnockbackForce = FVector::ZeroVector;
	UPROPERTY() bool bIsRadialDamage = false;
	UPROPERTY() float RadialDamageInnerRadius = 0.f;
	UPROPERTY() float RadialDamageOuterRadius = 0.f;
	UPROPERTY() FVector RadialDamageOrigin = FVector::ZeroVector;
	
	TSharedPtr<FGameplayTag> DamageType;
};

template<>
struct TStructOpsTypeTraits< FAuraGameplayEffectContext > : public TStructOpsTypeTraitsBase2< FAuraGameplayEffectContext >
{
	// override the inherited enums that we need
	enum
	{
		WithNetSerializer = true,
		WithCopy = true		// Necessary so that TSharedPtr<FHitResult> Data is copied around
	};
};