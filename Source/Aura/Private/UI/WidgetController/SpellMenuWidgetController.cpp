#include "UI/WidgetController/SpellMenuWidgetController.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)
{
	const FGameplayTag SelectedAbilityTpe = AbilityInfo->FindAbilityInfoForTag(AbilityTag).AbilityType;
	this->StopWaitForEquippedDelegate.Broadcast(SelectedAbilityTpe);

	const int32 SP = GetAuraPS()->GetSP();
	FGameplayTag AbilityStatus;
	const FAuraGameplayTags Tags = FAuraGameplayTags::Get();
	const FGameplayAbilitySpec* Spec = GetAuraASC()->GetSpecFromAbilityTag(AbilityTag);
	const bool bTagValid = AbilityTag.IsValid();
	const bool bTagNone = AbilityTag.MatchesTagExact(Tags.Abilities_None);
	const bool bSpecValid = Spec != nullptr;

	// Ability is locked or not valid
	if (!bTagValid || bTagNone || !bSpecValid)
	{
		AbilityStatus = Tags.Abilities_Status_Locked;
	}
	else
	{
		AbilityStatus = GetAuraASC()->GetStatusFromSpec(*Spec);
	}

	this->SelectedAbility.Ability = AbilityTag;
	this->SelectedAbility.Status = AbilityStatus;

	bool bEnableSPButton = false;
	bool bEnableEquipButton = false;
	FString Description;
	FString NextLevelDescription;
	this->EnableButtons(AbilityStatus, SP, bEnableSPButton, bEnableEquipButton);
	GetAuraASC()->GetDescriptionsByAbilityTag(AbilityTag, Description, NextLevelDescription);
	this->SpellGlobeSelectedDelegate.Broadcast(bEnableSPButton, bEnableEquipButton, Description, NextLevelDescription);
}

void USpellMenuWidgetController::SpendPointButtonPressed()
{
	GetAuraASC()->ServerSpendSpellPoint(this->SelectedAbility.Ability);
}

void USpellMenuWidgetController::GlobeDeselect()
{
	const FGameplayTag SelectedAbilityTpe = AbilityInfo->FindAbilityInfoForTag(this->SelectedAbility.Ability).AbilityType;
	this->StopWaitForEquippedDelegate.Broadcast(SelectedAbilityTpe);

	this->SelectedAbility.Ability = FAuraGameplayTags::Get().Abilities_None;
	this->SelectedAbility.Status = FAuraGameplayTags::Get().Abilities_Status_Locked;

	this->SpellGlobeSelectedDelegate.Broadcast(false, false, FString(), FString());
}

void USpellMenuWidgetController::EquippedButtonPressed()
{
	const FGameplayTag AbilityType = AbilityInfo->FindAbilityInfoForTag(this->SelectedAbility.Ability).AbilityType;
	this->WaitForEquippedDelegate.Broadcast(AbilityType);
	this->bWaitingForEquippedSelection = true;

	const FGameplayTag SelectedStatus = GetAuraASC()->GetStatusFromAbilityTag(this->SelectedAbility.Ability);
	if (SelectedStatus.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Equipped))
	{
		this->SelectedSlot = GetAuraASC()->GetSlotFromAbilityTag(this->SelectedAbility.Ability);
	}
}

void USpellMenuWidgetController::SpellGlobeRowPressed(const FGameplayTag& SlotTag, const FGameplayTag& AbilityType)
{
	if (!this->bWaitingForEquippedSelection) return;
	
	// check selected ability against the slot's ability type (don't equip passive in offensive and vice versa)
	const FGameplayTag& SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(this->SelectedAbility.Ability).AbilityType;
	if (!SelectedAbilityType.MatchesTagExact(AbilityType)) return;

	GetAuraASC()->ServerEquipAbility(this->SelectedAbility.Ability, SlotTag);
}

void USpellMenuWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot)
{
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();

	this->bWaitingForEquippedSelection = false;

	FAuraAbilityInfo LastSlotInfo;
	LastSlotInfo.StatusTag = GameplayTags.Abilities_Status_Unlocked;
	LastSlotInfo.InputTag = PreviousSlot;
	LastSlotInfo.AbilityTag = GameplayTags.Abilities_None;
	// Broadcast empty Info if previous slot is a valid slot. Only if equipping an already equipped spell
	this->AbilityInfoDelegate.Broadcast(LastSlotInfo);

	FAuraAbilityInfo CurrentSlotInfo = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	CurrentSlotInfo.StatusTag = Status;
	CurrentSlotInfo.InputTag = Slot;
	this->AbilityInfoDelegate.Broadcast(CurrentSlotInfo);

	this->StopWaitForEquippedDelegate.Broadcast(AbilityInfo->FindAbilityInfoForTag(AbilityTag).AbilityType);
	this->SpellGlobeReassignedDelegate.Broadcast(AbilityTag);
	this->GlobeDeselect();
}

void USpellMenuWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();
	this->SpellPointsChanged.Broadcast(GetAuraPS()->GetSP());
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	GetAuraASC()->StatusChanged.AddLambda(
		[this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 Level)
		{
			if (this->SelectedAbility.Ability.MatchesTagExact(AbilityTag))
			{
				this->SelectedAbility.Status = StatusTag;
				bool bEnableSPButton = false;
				bool bEnableEquipButton = false;
				this->EnableButtons(StatusTag, this->CurrentSP, bEnableSPButton, bEnableEquipButton);
				FString Description;
				FString NextLevelDescription;
				GetAuraASC()->GetDescriptionsByAbilityTag(this->SelectedAbility.Ability, Description, NextLevelDescription);
				this->SpellGlobeSelectedDelegate.Broadcast(bEnableSPButton, bEnableEquipButton, Description, NextLevelDescription);
			}

			if (this->AbilityInfo)
			{
				FAuraAbilityInfo Info = this->AbilityInfo->FindAbilityInfoForTag(AbilityTag);
				Info.StatusTag = StatusTag;
				this->AbilityInfoDelegate.Broadcast(Info);
			}
		}
	);

	GetAuraASC()->AbilityEquipped.AddUObject(this, &USpellMenuWidgetController::OnAbilityEquipped);

	GetAuraPS()->OnSpellPointsChangedDelegate.AddLambda(
		[this](int32 SpellPoints)
		{
			this->CurrentSP = SpellPoints;
			bool bEnableSPButton = false;
			bool bEnableEquipButton = false;
			this->EnableButtons(this->SelectedAbility.Status, SpellPoints, bEnableSPButton, bEnableEquipButton);
			FString Description;
			FString NextLevelDescription;
			GetAuraASC()->GetDescriptionsByAbilityTag(this->SelectedAbility.Ability, Description, NextLevelDescription);
			this->SpellGlobeSelectedDelegate.Broadcast(bEnableSPButton, bEnableEquipButton, Description, NextLevelDescription);
			this->SpellPointsChanged.Broadcast(SpellPoints);
		}
	);
}

void USpellMenuWidgetController::EnableButtons(const FGameplayTag& Status, int32 SP, bool& bShouldEnableSPButton, bool& bShouldEnableEquipButton)
{
	const FAuraGameplayTags Tags = FAuraGameplayTags::Get();
	
	if (Status.MatchesTagExact(Tags.Abilities_Status_Equipped))
	{
		bShouldEnableEquipButton = true;
		bShouldEnableSPButton = SP > 0 ? true : false;
	}
	else if (Status.MatchesTagExact(Tags.Abilities_Status_Eligible))
	{
		bShouldEnableEquipButton = false;
		bShouldEnableSPButton = SP > 0 ? true : false;
	}
	else if (Status.MatchesTagExact(Tags.Abilities_Status_Unlocked))
	{
		bShouldEnableEquipButton = true;
		bShouldEnableSPButton = SP > 0 ? true : false;
	}
	else if (Status.MatchesTagExact(Tags.Abilities_Status_Locked))
	{
		bShouldEnableEquipButton = false;
		bShouldEnableSPButton = false;
	}
}
