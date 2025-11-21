#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraPassiveAbilityBase.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraPassiveAbilityBase : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	void ReceiveDeactivate(const FGameplayTag& AbilityTag);

protected:

private:
};
