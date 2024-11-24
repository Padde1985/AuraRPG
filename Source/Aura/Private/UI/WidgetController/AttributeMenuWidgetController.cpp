#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "AuraGameplayTags.h"
#include "Player/AuraPlayerState.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"

void UAttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	GetAuraASC()->UpgradeAttribute(AttributeTag);
}

// broadcast initial values to be displayed in attribute menu
void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	check(this->AttributeInfo);

	// strange syntax for value part due to the function delegate
	for (TPair<FGameplayTag, FGameplayAttribute(*)()>& Pair : GetAuraAS()->TagsToAttributes)
	{
		this->BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}

	this->AttributePointsDelegate.Broadcast(GetAuraPS()->GetAP());
	this->SpellPointsDelegate.Broadcast(GetAuraPS()->GetSP());
}

// call lambda function to update values when changed
void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	for (TPair<FGameplayTag, FGameplayAttribute(*)()>& Pair : GetAuraAS()->TagsToAttributes)
	{
		GetAuraASC()->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this, Pair](const FOnAttributeChangeData& Data)
			{
				this->BroadcastAttributeInfo(Pair.Key, Pair.Value());
			}
		);
	}

	GetAuraPS()->OnAttributePointsChangedDelegate.AddLambda(
		[this](int32 Points)
		{
			this->AttributePointsDelegate.Broadcast(Points);
		}
	);
	GetAuraPS()->OnSpellPointsChangedDelegate.AddLambda(
		[this](int32 Points)
	{
			this->SpellPointsDelegate.Broadcast(Points);
	}
	);
}

// get value for attribute corresponding to tag and broadcast that information
void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const
{
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}
