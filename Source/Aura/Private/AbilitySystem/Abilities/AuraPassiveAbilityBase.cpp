#include "AbilitySystem/Abilities/AuraPassiveAbilityBase.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

void UAuraPassiveAbilityBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo())))
	{
		AuraASC->DeactivatePassiveAbility.AddUObject(this, &UAuraPassiveAbilityBase::ReceiveDeactivate);
	}
}

void UAuraPassiveAbilityBase::ReceiveDeactivate(const FGameplayTag& AbilityTag)
{
	//if (AbilityTags.HasTagExact(AbilityTag))
	if (GetAssetTags().HasTagExact(AbilityTag))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}
