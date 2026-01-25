#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AbilitySystem/AuraAttributeSet.h"

// getter for description on current level, will be overridden in child classes
FString UAuraGameplayAbility::GetDescription(int32 Level)
{
    return FString::Printf(TEXT("<Default>%s, </><Level>%d</>"), L"Default Ability", Level);
}

// getter for description of next level, will be overridden in all child classes
FString UAuraGameplayAbility::GetNextLevelDescription(int32 Level)
{
    return FString::Printf(TEXT("<Default>Next Level: </><Level>%d</>"), Level);
}

// get description when ability is still locked
FString UAuraGameplayAbility::GetLockedDescription(int32 Level)
{
    return FString::Printf(TEXT("<Default>Required Level: </><Level>%d</>"), Level);
}

// get Mana cost based on ability level from the data set
float UAuraGameplayAbility::GetManaCost(int32 Level) const
{
    float ManaCost = 0.f;
    if (const UGameplayEffect* CostEffect = GetCostGameplayEffect())
    {
        for (FGameplayModifierInfo Mod : CostEffect->Modifiers)
        {
            if (Mod.Attribute == UAuraAttributeSet::GetManaAttribute())
            {
                Mod.ModifierMagnitude.GetStaticMagnitudeIfPossible(float(Level), ManaCost);
                break;
            }
        }
    }
    return ManaCost * -1;
}

// get cooldwon from data set
float UAuraGameplayAbility::GetCooldown(int32 Level) const
{
    float Cooldown = 0.f;

    if (const UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect())
    {
        CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(float(Level), Cooldown);
    }

    return Cooldown;
}