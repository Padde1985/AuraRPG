#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraGameplayTags.h"

// bind callback for OnEffectApplied
void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	// trick 17: make the callback an RPC function and it will be called on server and client as the delegate only fires on the server otherwise
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::OnEffectApplied);
}

// add startup abilities to the ability component
void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartupInputTag);
			GiveAbility(AbilitySpec);
		}
	}

	this->bStartupAbilitiesGiven = true;
	this->AbilitiesGiven.Broadcast(this);
}

// react to keeping effect active via keeping the button pressed
void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (Spec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(Spec);
			if (!Spec.IsActive()) TryActivateAbility(Spec.Handle);
		}
	}
}

// react to stop button pressing
void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (Spec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputReleased(Spec);
		}
	}
}

void UAuraAbilitySystemComponent::ForEachAbility(const FForeachAbility& Delegate)
{
	FScopedAbilityListLock ActiveScopeLock(*this); // lock list of abilities so that they cannot be invalidated outside this function in parallel
	for (const FGameplayAbilitySpec& Spec : this->GetActivatableAbilities())
	{
		if (!Delegate.ExecuteIfBound(Spec))
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to execute Delegate in %hs"), __FUNCTION__);
		}
	}
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& Spec)
{
	if (Spec.Ability)
	{
		for (FGameplayTag Tag : Spec.Ability->AbilityTags)
		{
			// check if any Ability is assigned
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities"))))
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& Spec)
{
	for (FGameplayTag Tag : Spec.DynamicAbilityTags)
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
		{
			return Tag;
		}
	}
	return FGameplayTag();
}

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	if (!this->bStartupAbilitiesGiven)
	{
		this->bStartupAbilitiesGiven = true;
		this->AbilitiesGiven.Broadcast(this);
	}
}

// callback function for Effect applied (_implementation required because of RPC function)
void UAuraAbilitySystemComponent::OnEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);

	this->EffectAssetTags.Broadcast(TagContainer);
}
