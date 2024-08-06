#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "GameplayEffectTypes.h"

// broadcast initial values for Mana and Health
void UOverlayWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* Set = CastChecked<UAuraAttributeSet>(AttributeSet);

	this->OnHealthChanged.Broadcast(Set->GetHealth());
	this->OnMaxHealthChanged.Broadcast(Set->GetMaxHealth());

	this->OnManaChanged.Broadcast(Set->GetMana());
	this->OnMaxManaChanged.Broadcast(Set->GetMaxMana());
}

// bind callbacks for vital attributes and also bind callback to item pickup
void UOverlayWidgetController::BindCallbacksToDependencies()
{
	const UAuraAttributeSet* Set = CastChecked<UAuraAttributeSet>(AttributeSet);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Set->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)	{ this->OnHealthChanged.Broadcast(Data.NewValue); });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Set->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)	{ this->OnMaxHealthChanged.Broadcast(Data.NewValue); });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Set->GetManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)	{ this->OnManaChanged.Broadcast(Data.NewValue); });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Set->GetMaxManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)	{ this->OnMaxManaChanged.Broadcast(Data.NewValue); });
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTags.AddLambda(
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
}