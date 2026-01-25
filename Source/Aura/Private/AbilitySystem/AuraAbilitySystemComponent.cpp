#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraGameplayTags.h"
#include "Interaction/PlayerInterface.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitysystemLibrary.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Game/LoadMenuSaveGame.h"

// handle ability upgrade
void UAuraAbilitySystemComponent::ServerUpgradeAttribute_Implementation(const FGameplayTag& AttributeTag)
{
	FGameplayEventData Payload;
	Payload.EventTag = AttributeTag;
	Payload.EventMagnitude = 1;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);

	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		IPlayerInterface::Execute_AddToAttributePoints(GetAvatarActor(), -1);
	}
}

// handle spell point change when spending points for abilities
void UAuraAbilitySystemComponent::ServerSpendSpellPoint_Implementation(const FGameplayTag& AbilityTag)
{
	if (FGameplayAbilitySpec* Spec = this->GetSpecFromAbilityTag(AbilityTag))
	{
		const FAuraGameplayTags Tags = FAuraGameplayTags::Get();
		FGameplayTag Status = this->GetStatusFromSpec(*Spec);

		if (GetAvatarActor()->Implements<UPlayerInterface>()) IPlayerInterface::Execute_AddToSpellPoints(GetAvatarActor(), -1);

		if (Status.MatchesTagExact(Tags.Abilities_Status_Eligible))
		{
			Spec->GetDynamicSpecSourceTags().RemoveTag(Tags.Abilities_Status_Eligible);
			Spec->GetDynamicSpecSourceTags().AddTag(Tags.Abilities_Status_Unlocked);
			Status = Tags.Abilities_Status_Unlocked;
		}
		else if (Status.MatchesTagExact(Tags.Abilities_Status_Equipped) || Status.MatchesTagExact(Tags.Abilities_Status_Unlocked))
		{
			Spec->Level++;
		}
		this->ClientUpdateAbilityStatus(AbilityTag, Status, Spec->Level);
		MarkAbilitySpecDirty(*Spec); // force to update immediately
	}
}

// handle equipping an ability
void UAuraAbilitySystemComponent::ServerEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& Slot)
{
	if (FGameplayAbilitySpec* AbilitySpec = this->GetSpecFromAbilityTag(AbilityTag))
	{
		const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
		const FGameplayTag& PreviousSlot = this->GetInputTagFromSpec(*AbilitySpec);
		const FGameplayTag& Status = this->GetStatusFromSpec(*AbilitySpec);

		const bool bStatusValid = Status == GameplayTags.Abilities_Status_Equipped || Status == GameplayTags.Abilities_Status_Unlocked;
		if (bStatusValid)
		{
			// Handle activation/deactivation for passive abilities
			// selected UI-Slot is not empty, we have to replace the current ability
			if (!this->SlotIsEmpty(Slot))
			{
				FGameplayAbilitySpec* SpecWithSlot = this->GetSpecWithSlot(Slot);
				if (SpecWithSlot)
				{
					// is that ability the same as already equipped
					if (AbilityTag.MatchesTagExact(this->GetAbilityTagFromSpec(*SpecWithSlot)))
					{
						this->ClientEquipAbility(AbilityTag, GameplayTags.Abilities_Status_Equipped, Slot, PreviousSlot);
						return;
					}
					
					if (this->IsPassiveAbility(*SpecWithSlot))
					{
						// first deactivate the current Slot's Niagara component
						this->MulticastActivatePassiveEffect(this->GetAbilityTagFromSpec(*SpecWithSlot), false);
						this->DeactivatePassiveAbility.Broadcast(GetAbilityTagFromSpec(*SpecWithSlot));
					}
					//change status from equipped to unlocked for passive and offensive abilities
					SpecWithSlot->GetDynamicSpecSourceTags().RemoveTag(this->GetStatusFromSpec(*SpecWithSlot));
					SpecWithSlot->GetDynamicSpecSourceTags().AddTag(GameplayTags.Abilities_Status_Unlocked);

					this->ClearSlot(SpecWithSlot);
				}
			}

			if (!this->AbilityHasAnySlot(*AbilitySpec)) // Ability does not have any Slot yet, is nowhere assigned yet
			{
				if (this->IsPassiveAbility(*AbilitySpec))
				{
					TryActivateAbility(AbilitySpec->Handle);
					// activate the new Niagara component
					this->MulticastActivatePassiveEffect(AbilityTag, true);
					// to take some effect, grant a Tag here and then check it in damage effect calc to reduce damage, etc.
					// to get health every so often, the tag then has to be checked in the attribute set
				}
				// when assigning to any slot the first time, set status to equipped
				AbilitySpec->GetDynamicSpecSourceTags().RemoveTag(GetStatusFromSpec(*AbilitySpec));
				AbilitySpec->GetDynamicSpecSourceTags().AddTag(GameplayTags.Abilities_Status_Equipped);
			}
			this->AssignSlotToAbility(*AbilitySpec, Slot);

			MarkAbilitySpecDirty(*AbilitySpec);
		}
		this->ClientEquipAbility(AbilityTag, GameplayTags.Abilities_Status_Equipped, Slot, PreviousSlot);
	}
}

// broadcast equipping ability on multiplayer as clients only react to server changes
void UAuraAbilitySystemComponent::ClientEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot)
{
	this->AbilityEquipped.Broadcast(AbilityTag, Status, Slot, PreviousSlot);
}

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
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(AuraAbility->StartupInputTag);
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(FAuraGameplayTags::Get().Abilities_Status_Equipped);
			GiveAbility(AbilitySpec);
		}
	}

	this->bStartupAbilitiesGiven = true;
	this->AbilitiesGiven.Broadcast();
}

// add passive abilities to character and activate right away
void UAuraAbilitySystemComponent::AddPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Passives)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : Passives)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(FAuraGameplayTags::Get().Abilities_Status_Equipped);
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
}

// react to keeping effect active via keeping the button pressed
void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	FScopedAbilityListLock ScopedAbilityListLock(*this);
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		//if (Spec.DynamicAbilityTags.HasTagExact(InputTag))
		if (Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
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

	// Lock the ActivatableAbilities container until the for loop is done.
	FScopedAbilityListLock ScopedAbilityListLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			AbilitySpecInputReleased(AbilitySpec);
			if (AbilitySpec.IsActive())
			{
				TArray<UGameplayAbility*> AbilityInstances = AbilitySpec.GetAbilityInstances();
				for (UGameplayAbility* AbilityInstance : AbilityInstances)
				{
					InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, AbilitySpec.Handle, AbilityInstance->GetCurrentActivationInfo().GetActivationPredictionKey());
				}
			}
		}
	}
}

// react to button assigedn to an ability
void UAuraAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	// Lock the ActivatableAbilities container until the for loop is done.
	FScopedAbilityListLock ScopedAbilityListLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (AbilitySpec.IsActive())
			{
				TArray<UGameplayAbility*> AbilityInstances = AbilitySpec.GetAbilityInstances();
				for (UGameplayAbility* AbilityInstance : AbilityInstances)
				{
					InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, AbilitySpec.Handle, AbilityInstance->GetCurrentActivationInfo().GetActivationPredictionKey());
				}
			}
		}
	}
}

// loop over all abilities
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

// get gameplay tag from given ability
FGameplayTag UAuraAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& Spec)
{
	if (Spec.Ability)
	{
		for (FGameplayTag Tag : Spec.Ability->GetAssetTags())
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

// get input tag from given ability
FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& Spec)
{
	for (FGameplayTag Tag : Spec.GetDynamicSpecSourceTags())
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
		{
			return Tag;
		}
	}
	return FGameplayTag();
}

// upgrade attribute change (Vigor, Intelligence, etc.)
void UAuraAbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		if (IPlayerInterface::Execute_GetAttributePoints(GetAvatarActor()) > 0)
		{
			this->ServerUpgradeAttribute(AttributeTag);
		}
	}
}

// get ability status
FGameplayTag UAuraAbilitySystemComponent::GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag StatusTag : AbilitySpec.GetDynamicSpecSourceTags())
	{
		if (StatusTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Status")))) return StatusTag;
	}
	return FGameplayTag();
}

// update ability status
void UAuraAbilitySystemComponent::UpdateAbilityStatuses(int32 Level)
{
	UAbilityInfo* AbilityInfo = UAuraAbilitysystemLibrary::GetAbilityInfo(GetAvatarActor());
	for (const FAuraAbilityInfo& Info : AbilityInfo->AbilityInformation)
	{
		if (Level < Info.LevelRequirement || !Info.AbilityTag.IsValid()) continue;

		if (this->GetSpecFromAbilityTag(Info.AbilityTag) == nullptr)
		{
			FGameplayAbilitySpec Spec = FGameplayAbilitySpec(Info.Ability, 1);
			Spec.GetDynamicSpecSourceTags().AddTag(FAuraGameplayTags::Get().Abilities_Status_Eligible);
			GiveAbility(Spec);
			MarkAbilitySpecDirty(Spec); // forces to replicate immediately
			this->ClientUpdateAbilityStatus(Info.AbilityTag, FAuraGameplayTags::Get().Abilities_Status_Eligible, 1);
		}
	}
}

// get ability from gameplay tag
FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecFromAbilityTag(const FGameplayTag& AbilityTag)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		for (FGameplayTag Tag : Spec.Ability.Get()->GetAssetTags())
		{
			if (Tag.MatchesTagExact(AbilityTag)) return &Spec;
		}
	}

	return nullptr;
}

// get descriptions for given ability
bool UAuraAbilitySystemComponent::GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription, FString& OutNextLevelDescription)
{
	if (const FGameplayAbilitySpec* Spec = this->GetSpecFromAbilityTag(AbilityTag))
	{
		if (UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(Spec->Ability))
		{
			OutDescription = AuraAbility->GetDescription(Spec->Level);
			OutNextLevelDescription = AuraAbility->GetNextLevelDescription(Spec->Level + 1);
			
			return true;
		}
	}

	if (AbilityTag.IsValid() || AbilityTag.MatchesTagExact(FAuraGameplayTags::Get().Abilities_None))
	{
		OutDescription = FString();
	}
	else
	{
		const UAbilityInfo* AbilityInfo = UAuraAbilitysystemLibrary::GetAbilityInfo(GetAvatarActor());
		OutDescription = UAuraGameplayAbility::GetLockedDescription(AbilityInfo->FindAbilityInfoForTag(AbilityTag).LevelRequirement);
	}
	OutNextLevelDescription = FString();
	
	return false;
}

// get status from ability
FGameplayTag UAuraAbilitySystemComponent::GetStatusFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* Spec = this->GetSpecFromAbilityTag(AbilityTag))
	{
		return this->GetStatusFromSpec(*Spec);
	}
	return FGameplayTag();
}

// get slot information from ability
FGameplayTag UAuraAbilitySystemComponent::GetSlotFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* Spec = this->GetSpecFromAbilityTag(AbilityTag))
	{
		return this->GetInputTagFromSpec(*Spec);
	}
	return FGameplayTag();
}

// clear the slot if unequipped
void UAuraAbilitySystemComponent::ClearSlot(FGameplayAbilitySpec* Spec)
{
	const FGameplayTag Slot = GetInputTagFromSpec(*Spec);
	Spec->GetDynamicSpecSourceTags().RemoveTag(Slot);
}

// clear any ability of a given slot
void UAuraAbilitySystemComponent::ClearAbilitiesOfSlot(const FGameplayTag& Slot)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (this->AbilityHasSlot(&Spec, Slot))
		{
			this->ClearSlot(&Spec);
		}
	}
}

// check if the ability is assigned to any slot
bool UAuraAbilitySystemComponent::AbilityHasSlot(FGameplayAbilitySpec* Spec, const FGameplayTag& Slot)
{
	for (const FGameplayTag& Tag : Spec->GetDynamicSpecSourceTags())
	{
		if (Tag.MatchesTagExact(Slot)) return true;
	}
	return false;
}

// get data from save game and apply to character
void UAuraAbilitySystemComponent::AddCharacterAbilitiesFromSaveData(ULoadMenuSaveGame* SaveData)
{
	FAuraGameplayTags Tags = FAuraGameplayTags::Get();
	for (const FSavedAbility& Data : SaveData->SavedAbilities)
	{
		const TSubclassOf<UGameplayAbility> LoadedAbilityClass = Data.GameplayAbility;

		FGameplayAbilitySpec LoadedAbilitySpec = FGameplayAbilitySpec(LoadedAbilityClass, Data.Level);
		LoadedAbilitySpec.GetDynamicSpecSourceTags().AddTag(Data.AbilitySlot);
		LoadedAbilitySpec.GetDynamicSpecSourceTags().AddTag(Data.AbilityStatus);

		if (Data.AbilityType == Tags.Abilities_Type_Offensive)
		{
			GiveAbility(LoadedAbilitySpec);
		}
		else if(Data.AbilityType == Tags.Abilities_Type_Passive)
		{
			GiveAbility(LoadedAbilitySpec);
			if (Data.AbilityStatus.MatchesTagExact(Tags.Abilities_Status_Equipped))
			{
				TryActivateAbility(LoadedAbilitySpec.Handle);
			}
		}
	}
	this->bStartupAbilitiesGiven = true;
	this->AbilitiesGiven.Broadcast();
}

// check if selected slot is empty
bool UAuraAbilitySystemComponent::SlotIsEmpty(const FGameplayTag& Slot)
{
	FScopedAbilityListLock ScopedAbilityListLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (this->AbilityHasSlot(AbilitySpec, Slot)) return false;
	}

	return true;
}

// check if given ability is assigned to a specific slot
bool UAuraAbilitySystemComponent::AbilityHasSlot(const FGameplayAbilitySpec& Spec, const FGameplayTag& Slot)
{
	return Spec.GetDynamicSpecSourceTags().HasTagExact(Slot);
}

// check if ability is assigned anywhere
bool UAuraAbilitySystemComponent::AbilityHasAnySlot(const FGameplayAbilitySpec& Spec)
{
	return Spec.GetDynamicSpecSourceTags().HasTag(FGameplayTag::RequestGameplayTag(FName("InputTag")));
}

// get the ability spec from slot
FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecWithSlot(const FGameplayTag& Slot)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(Slot)) return &AbilitySpec;
	}

	return nullptr;
}

// check if an ability is a passive one
bool UAuraAbilitySystemComponent::IsPassiveAbility(const FGameplayAbilitySpec& Spec) const
{
	UAbilityInfo* AbilityInfo = UAuraAbilitysystemLibrary::GetAbilityInfo(GetAvatarActor());
	const FGameplayTag AbilityTag = GetAbilityTagFromSpec(Spec);
	const FAuraAbilityInfo& Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	const FGameplayTag AbilityType = Info.AbilityType;
	
	return AbilityType.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Type_Passive);
}

// assign the ability to a slot
void UAuraAbilitySystemComponent::AssignSlotToAbility(FGameplayAbilitySpec& Spec, const FGameplayTag& Slot)
{
	ClearSlot(&Spec);
	Spec.GetDynamicSpecSourceTags().AddTag(Slot);
}

// client informs the server about a change on the ability status
void UAuraAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel)
{
	this->StatusChanged.Broadcast(AbilityTag, StatusTag, AbilityLevel);
}

// broadcast change on passive effect status
void UAuraAbilitySystemComponent::MulticastActivatePassiveEffect_Implementation(const FGameplayTag& AbilityTag, bool bActivate)
{
	this->ActivatePassiveEffect.Broadcast(AbilityTag, bActivate);
}

// replication for ability activation, only needed in multiplayer
void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	if (!this->bStartupAbilitiesGiven)
	{
		this->bStartupAbilitiesGiven = true;
		this->AbilitiesGiven.Broadcast();
	}
}

// callback function for Effect applied (_implementation required because of RPC function)
void UAuraAbilitySystemComponent::OnEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);

	this->EffectAssetTags.Broadcast(TagContainer);
}