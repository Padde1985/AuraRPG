#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "AuraGameplayTags.h"
#include "../AuraAttributeSet.h"
#include "ExecCalc_Damage.generated.h"

struct AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CritChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CritDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CritResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance);

	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;

	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CritChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CritDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CritResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, FireResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, LightningResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArcaneResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, PhysicalResistance, Target, false);
	}

	void InitializeTagsToCaptureDefsMap()
	{
		const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();
		TagsToCaptureDefs.Empty();

		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_Armor, ArmorDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_ArmorPenetration, ArmorPenetrationDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_BlockChance, BlockChanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CritChance, CritChanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CritDamage, CritDamageDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CritResistance, CritResistanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Fire, FireResistanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Lightning, LightningResistanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Arcane, ArcaneResistanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Physical, PhysicalResistanceDef);
	}
};

// Static functions keep their values with each call
static AuraDamageStatics& DamageStatics()
{
	static AuraDamageStatics DStatics;
	return DStatics;
}

static void InitializeStaticTagsToCaptureDefsMap()
{
	DamageStatics().InitializeTagsToCaptureDefsMap();
}

UCLASS()
class AURA_API UExecCalc_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UExecCalc_Damage();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
