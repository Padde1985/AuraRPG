#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /*Asset Tags*/);
DECLARE_MULTICAST_DELEGATE(FAbilitiesGiven);
DECLARE_DELEGATE_OneParam(FForeachAbility, const FGameplayAbilitySpec&);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FAbilityStatusChanged, const FGameplayTag& /*AbilityTag*/, const FGameplayTag& /*StatusTag*/, int32 /*AbilityLevel*/);
DECLARE_MULTICAST_DELEGATE_FourParams(FAbilityEquipped, const FGameplayTag& /*AbilityTag*/, const FGameplayTag& /*StatusTag*/, const FGameplayTag& /*Slot*/, const FGameplayTag& /*Previous Slot*/)
DECLARE_MULTICAST_DELEGATE_OneParam(FDeactivatePassiveAbility, const FGameplayTag& /*AbilityTag*/);
DECLARE_MULTICAST_DELEGATE_TwoParams(FActivatePassiveEffect, const FGameplayTag& /*AbilityTag*/, bool /*bActivate*/);

UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(Server, Reliable) void ServerUpgradeAttribute(const FGameplayTag& AttributeTag);
	UFUNCTION(Server, Reliable) void ServerSpendSpellPoint(const FGameplayTag& AbilityTag);
	UFUNCTION(Server, Reliable) void ServerEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& Slot);
	UFUNCTION(Client, Reliable) void ClientEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot);

	FEffectAssetTags EffectAssetTags;
	FAbilitiesGiven AbilitiesGiven;
	FAbilityStatusChanged StatusChanged;
	FAbilityEquipped AbilityEquipped;
	FDeactivatePassiveAbility DeactivatePassiveAbility;
	FActivatePassiveEffect ActivatePassiveEffect;
	
	bool bStartupAbilitiesGiven = false;

	void AbilityActorInfoSet();
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);
	void AddPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Passives);
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void ForEachAbility(const FForeachAbility& Delegate);
	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& Spec);
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& Spec);
	void UpgradeAttribute(const FGameplayTag& AttributeTag);
	static FGameplayTag GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	void UpdateAbilityStatuses(int32 Level);
	FGameplayAbilitySpec* GetSpecFromAbilityTag(const FGameplayTag& AbilityTag);
	bool GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription, FString& OutNextLevelDescription);
	FGameplayTag GetStatusFromAbilityTag(const FGameplayTag& AbilityTag);
	FGameplayTag GetSlotFromAbilityTag(const FGameplayTag& AbilityTag);
	static void ClearSlot(FGameplayAbilitySpec* Spec);
	void ClearAbilitiesOfSlot(const FGameplayTag& Slot);
	static bool AbilityHasSlot(FGameplayAbilitySpec* Spec, const FGameplayTag& Slot);

protected:
	UFUNCTION(Client, Reliable) void OnEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);
	UFUNCTION(Client, Reliable) void ClientUpdateAbilityStatus(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel);
	UFUNCTION(NetMulticast, Unreliable) void MulticastActivatePassiveEffect(const FGameplayTag& AbilityTag, bool bActivate);

	virtual void OnRep_ActivateAbilities() override; // needed to call broadcasts on clients

private:
	bool SlotIsEmpty(const FGameplayTag& Slot);
	static bool AbilityHasSlot(const FGameplayAbilitySpec& Spec, const FGameplayTag& Slot);
	static bool AbilityHasAnySlot(const FGameplayAbilitySpec& Spec);
	FGameplayAbilitySpec* GetSpecWithSlot(const FGameplayTag& Slot);
	bool IsPassiveAbility(const FGameplayAbilitySpec& Spec) const;
	static void AssignSlotToAbility(FGameplayAbilitySpec& Spec, const FGameplayTag& Slot);
};
