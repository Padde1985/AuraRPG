#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "AbilitySystem/AuraAbilitysystemLibrary.h"
#include "Interaction/CombatInterface.h"
#include "AuraAbilityTypes.h"
#include "Kismet/GameplayStatics.h"

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
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();

	// Gather tags from source and target
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluationParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	// Determine Debufffs
	this->DetermineDebuff(Spec, ExecutionParams, EvaluationParameters);

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
		if (DamageTypeValue <= 0.f) continue;
		// Damage * Resistance in percent
		DamageTypeValue *= ((100.f - ResistanceValue) / 100.f);

		// 1.1 check Radial Damage
		if (UAuraAbilitysystemLibrary::IsRadialDamage(EffectContextHandle))
		{
			if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(TargetAvatar))
			{
				CombatInterface->GetOnDamageDelegate().AddLambda([&](float DamageAmount) // & is capturing just everything by reference
				{
					DamageTypeValue = DamageAmount;
				});
			}

			// calling the following function will trigger TakeDamage on the victim and therefore call the DamageDelegate to propagate the damage value
			UGameplayStatics::ApplyRadialDamageWithFalloff(
				TargetAvatar, DamageTypeValue, 0.f,
				UAuraAbilitysystemLibrary::GetRadialDamageOrigin(EffectContextHandle),
				UAuraAbilitysystemLibrary::GetRadialDamageInnerRadius(EffectContextHandle),
				UAuraAbilitysystemLibrary::GetRadialDamageOuterRadius(EffectContextHandle),
				1.f, UDamageType::StaticClass(),
				TArray<AActor*>(),
				SourceAvatar, nullptr
			);
		}

		Damage += DamageTypeValue;
	}

	// 2.0. Consider Block Chance
	float TargetBlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParameters, TargetBlockChance);
	TargetBlockChance = FMath::Max<float>(TargetBlockChance, 0.f);

	const bool bIsBlocked = FMath::RandRange(1, 100) < TargetBlockChance;
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

	// 6. Check for Passive Spell gameplaytags and adjust damage output

	// 5. apply critical hits
	const float EffectiveCritChance = SourceCritChance - TargetCritResistance * EffectiveCritCoeff;
	const bool bIsCriticalHit = FMath::RandRange(1, 100) < EffectiveCritChance;
	UAuraAbilitysystemLibrary::SetIsCriticalHit(EffectContextHandle, bIsCriticalHit);

	Damage = bIsCriticalHit ? Damage * 2.f + SourceCritDamage : Damage;

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage));
}

void UExecCalc_Damage::DetermineDebuff(const FGameplayEffectSpec& Spec, const FGameplayEffectCustomExecutionParameters& ExecutionParams, FAggregatorEvaluateParameters& EvaluationParameters) const
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	for (const TTuple<FGameplayTag, FGameplayTag> Pair : GameplayTags.DamageTypesToDebuffs)
	{
		const FGameplayTag& DamageType = Pair.Key;
		const FGameplayTag& DebuffType = Pair.Value;
		const float TypeDamage = Spec.GetSetByCallerMagnitude(DamageType, false, -1.f);
		if (TypeDamage > -1.f)
		{
			// Determine if there was a successful debuff
			const float SourceDebuffChance = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Chance, false, -1.f);
			float TargetDebuffResistance = 0.f;
			const FGameplayTag& ResistanceTag = GameplayTags.DamageTypesToResistances[DamageType];
			ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().TagsToCaptureDefs[ResistanceTag], EvaluationParameters, TargetDebuffResistance);
			TargetDebuffResistance = FMath::Max<float>(TargetDebuffResistance, 0.f);

			const float EffectiveDebuffChance = SourceDebuffChance * (100 - TargetDebuffResistance) / 100;
			const bool bDebuff = FMath::RandRange(1, 100) < EffectiveDebuffChance;
			if (bDebuff)
			{
				FGameplayEffectContextHandle ContextHandle = Spec.GetContext();
				UAuraAbilitysystemLibrary::SetIsDebuffSuccessful(ContextHandle, bDebuff);

				const float DebuffDamage = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Damage, false, -1.f);
				const float DebuffDuration = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Duration, false, -1.f);
				const float DebuffFrequency = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Frequency, false, -1.f);

				UAuraAbilitysystemLibrary::SetDebuffDamage(ContextHandle, DebuffDamage);
				UAuraAbilitysystemLibrary::SetDebuffDuration(ContextHandle, DebuffDuration);
				UAuraAbilitysystemLibrary::SetDebuffFrequency(ContextHandle, DebuffFrequency);
				UAuraAbilitysystemLibrary::SetDamageType(ContextHandle, DamageType);
			}
		}
	}
}
