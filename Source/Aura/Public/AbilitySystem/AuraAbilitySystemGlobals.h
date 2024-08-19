#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "AuraAbilitySystemGlobals.generated.h"

// to use this class it has to be inserted into the config file DefaultGame.ini as follows
//[/Script/GameplayAbilities.AbilitySystemGlobals]
//+AbilitySystemGlobalsClassName = "/Script/<ProjectName>.<Class name of Globals class without the "U" in front>"
UCLASS()
class AURA_API UAuraAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()
	
public:
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
};
