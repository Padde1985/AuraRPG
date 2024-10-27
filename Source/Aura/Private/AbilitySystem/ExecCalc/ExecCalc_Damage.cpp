#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "AbilitySystem/AuraAbilitysystemLibrary.h"
#include "Interaction/CombatInterface.h"
#include "AuraAbilityTypes.h"

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().CritChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CritDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().CritResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().FireResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().LightningResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArcaneResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalResistanceDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	int32 SourcePlayerLevel = 1;
	if (SourceAvatar->Implements<UCombatInterface>())
	{
		SourcePlayerLevel = ICombatInterface::Execute_GetLevel(SourceAvatar);
	}
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
	int32 TargetPlayerLevel = 1;
	if (TargetAvatar->Implements<UCombatInterface>())
	{
		TargetPlayerLevel = ICombatInterface::Execute_GetLevel(TargetAvatar);
	}

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	// Gather tags from source and target
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluationParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	// 1. get Damage for all types
	float Damage = 0;
	for (const TPair<FGameplayTag, FGameplayTag>& DamageTypeToResistance : FAuraGameplayTags::Get().DamageTypesToResistances)
	{
		const FGameplayTag DamageType = DamageTypeToResistance.Key;
		const FGameplayTag Resistance = DamageTypeToResistance.Value;

		checkf(DamageStatics().TagsToCaptureDefs.Contains(Resistance), TEXT("TagsToCaptureDefs doesn't contain Tag [%s] in ExecCalc_Damage"), *Resistance.ToString());

		const FGameplayEffectAttributeCaptureDefinition CaptureDef = DamageStatics().TagsToCaptureDefs[Resistance];

		float ResistanceValue = 0;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluationParameters, ResistanceValue);
		ResistanceValue = FMath::Clamp(ResistanceValue, 0.f, 100);

		// Get Damage set by caller magnitude and calculate Block chance
		float DamageTypeValue = Spec.GetSetByCallerMagnitude(DamageTypeToResistance.Key, false);
		// Damage * Resistance in percent
		DamageTypeValue *= ((100.f - ResistanceValue) / 100.f);
		Damage += DamageTypeValue;
	}

	// 2.0. Consider Block Chance
	float TargetBlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParameters, TargetBlockChance);
	TargetBlockChance = FMath::Max<float>(TargetBlockChance, 0.f);

	const bool bIsBlocked = FMath::RandRange(1, 100) < TargetBlockChance;
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
	UAuraAbilitysystemLibrary::SetIsBlockedHit(EffectContextHandle, bIsBlocked);

	Damage = bIsBlocked ? Damage / 2.f : Damage;

	// 2.1 Consider Armor
	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, TargetArmor);
	TargetArmor = FMath::Max<float>(TargetArmor, 0.f);

	// 2.2 Consider Armor Penetration from spell casting character
	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluationParameters, SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max<float>(SourceArmorPenetration, 0.f);

	// 3. Consider Crit attributes
	float SourceCritChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CritChanceDef, EvaluationParameters, SourceCritChance);
	SourceCritChance = FMath::Max<float>(SourceCritChance, 0.f);
	float SourceCritDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CritDamageDef, EvaluationParameters, SourceCritDamage);
	SourceCritDamage = FMath::Max<float>(SourceCritDamage, 0.f);
	float TargetCritResistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CritResistanceDef, EvaluationParameters, TargetCritResistance);
	TargetCritResistance = FMath::Max<float>(TargetCritResistance, 0.f);

	// 4. get coefficients
	const UCharacterClassInfo* CharacterClassInfo = UAuraAbilitysystemLibrary::GetCharacterClassInfo(SourceAvatar); // "this" does not work as this class is not an object in the world
	const FRealCurve* ArmorPenetrationCurve = CharacterClassInfo->DamageCalculationCoeffs->FindCurve(FName("ArmorPenetration"), FString());
	const float ArmorPenetrationCoeff = ArmorPenetrationCurve->Eval(SourcePlayerLevel);
	const FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCalculationCoeffs->FindCurve(FName("EffectiveArmor"), FString());
	const float EffectiveArmorCoeff = EffectiveArmorCurve->Eval(TargetPlayerLevel);
	const FRealCurve* EffectiveCritCurve = CharacterClassInfo->DamageCalculationCoeffs->FindCurve(FName("CritResistance"), FString());
	const float EffectiveCritCoeff = EffectiveCritCurve->Eval(TargetPlayerLevel);

	const float EffectiveArmor = TargetArmor * (100 - SourceArmorPenetration * ArmorPenetrationCoeff) / 100.f;
	Damage *= (100 - EffectiveArmor / EffectiveArmorCoeff) / 100.f;

	// 5. apply critical hits
	const float EffectiveCritChance = SourceCritChance - TargetCritResistance * EffectiveCritCoeff;
	const bool bIsCriticalHit = FMath::RandRange(1, 100) < EffectiveCritChance;
	UAuraAbilitysystemLibrary::SetIsCriticalHit(EffectContextHandle, bIsCriticalHit);

	Damage = bIsCriticalHit ? Damage * 2.f + SourceCritDamage : Damage;

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage));
}
