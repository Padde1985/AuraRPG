#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"
#include "AbilitySystem/Data/LevelInfo.h"
#include "AuraGameplayTags.h"

// broadcast initial values for Mana and Health
void UOverlayWidgetController::BroadcastInitialValues()
{
	this->OnHealthChanged.Broadcast(GetAuraAS()->GetHealth());
	this->OnMaxHealthChanged.Broadcast(GetAuraAS()->GetMaxHealth());

	this->OnManaChanged.Broadcast(GetAuraAS()->GetMana());
	this->OnMaxManaChanged.Broadcast(GetAuraAS()->GetMaxMana());
}

// bind callbacks for vital attributes and also bind callback to item pickup
void UOverlayWidgetController::BindCallbacksToDependencies()
{
	GetAuraPS()->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnXPChanged);
	GetAuraPS()->OnLevelChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnLevelChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)	{ this->OnHealthChanged.Broadcast(Data.NewValue); });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)	{ this->OnMaxHealthChanged.Broadcast(Data.NewValue); });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)	{ this->OnManaChanged.Broadcast(Data.NewValue); });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetMaxManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)	{ this->OnMaxManaChanged.Broadcast(Data.NewValue); });
	if (GetAuraASC())
	{
		GetAuraASC()->AbilityEquipped.AddUObject(this, &UOverlayWidgetController::OnAbilityEquipped);

		GetAuraASC()->EffectAssetTags.AddLambda(
			[this](const FGameplayTagContainer& AssetTags)
		{
			for (const FGameplayTag& Tag : AssetTags)
			{
				// only Broadcast Message tags
				FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
				if (Tag.MatchesTag(MessageTag))
				{
					const FUIWidgetRow* Row = this->GetDataTableRowByTag<FUIWidgetRow>(this->MessageWidgetDataTable, Tag);
					this->MessageWidgetRowDelegate.Broadcast(*Row);
				}
			}
		}
		);

		// if Abilities were given first we have to call the function directly
		if (GetAuraASC()->bStartupAbilitiesGiven)
		{
			BroadcastAbilityInfo();
		}
		// otherwise just bind to callback and wait for Abilities to be set
		else
		{
			GetAuraASC()->AbilitiesGiven.AddUObject(this, &UOverlayWidgetController::BroadcastAbilityInfo);
		}
	}
}

void UOverlayWidgetController::OnXPChanged(int32 NewXP)
{
	const ULevelInfo* LevelInfo = GetAuraPS()->LevelInfo;

	checkf(LevelInfo, TEXT("Unable to find LevelInfo, please fill out AuraPlayerState Blueprint"));

	const int32 Level = LevelInfo->FindLevelForXP(NewXP);
	const int32 MaxLevel = LevelInfo->LevelInformation.Last().Level;
	if (Level <= MaxLevel)
	{
		const int32 LevelUpRequirement = LevelInfo->GetInfoForLevel(Level).LevelUpRequirement;
		const int32 PreviousLevelUpRequirement = Level > 2 ? LevelInfo->GetInfoForLevel(Level - 1).LevelUpRequirement : 0;		
		const int32 Delta = LevelUpRequirement - PreviousLevelUpRequirement;
		const int32 XPLevel = NewXP - PreviousLevelUpRequirement;
		const float XPBarPercent = static_cast<float>(XPLevel) / static_cast<float>(Delta);

		this->OnXPPercentChanged.Broadcast(XPBarPercent);
	}
}

void UOverlayWidgetController::OnLevelChanged(int32 NewLevel)
{
	this->OnPlayerLevelChangedDelegate.Broadcast(NewLevel);
}

void UOverlayWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot) const
{
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();

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
}
