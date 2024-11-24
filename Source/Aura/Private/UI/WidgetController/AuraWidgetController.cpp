#include "UI/WidgetController/AuraWidgetController.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"

void UAuraWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	this->PlayerController = WCParams.PlayerController;
	this->PlayerState = WCParams.PlayerState;
	this->AbilitySystemComponent = WCParams.AbilitySystemComponent;
	this->AttributeSet = WCParams.AttributeSet;
}

void UAuraWidgetController::BroadcastInitialValues()
{
}

void UAuraWidgetController::BindCallbacksToDependencies()
{
}

void UAuraWidgetController::BroadcastAbilityInfo()
{
	// Get information about all given abilities, look up ABilityInfo, broadcast to widgets
	if (!GetAuraASC()->bStartupAbilitiesGiven) return;

	FForeachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda(
		[this](const FGameplayAbilitySpec& Spec)
	{
		// figure out the ability for a given tag
		FAuraAbilityInfo Info = this->AbilityInfo->FindAbilityInfoForTag(GetAuraASC()->GetAbilityTagFromSpec(Spec));
		Info.InputTag = GetAuraASC()->GetInputTagFromSpec(Spec);
		Info.StatusTag = GetAuraASC()->GetStatusFromSpec(Spec);
		AbilityInfoDelegate.Broadcast(Info);
	}
	);
	GetAuraASC()->ForEachAbility(BroadcastDelegate);
}

AAuraPlayerController* UAuraWidgetController::GetAuraPC()
{
	if (this->AuraPlayerController == nullptr) this->AuraPlayerController = Cast<AAuraPlayerController>(this->PlayerController);
	return this->AuraPlayerController;
}

AAuraPlayerState* UAuraWidgetController::GetAuraPS()
{
	if (this->AuraPlayerState == nullptr) this->AuraPlayerState = Cast<AAuraPlayerState>(this->PlayerState);
	return this->AuraPlayerState;
}

UAuraAbilitySystemComponent* UAuraWidgetController::GetAuraASC()
{
	if (this->AuraAbilitySystemComponent == nullptr) this->AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(this->AbilitySystemComponent);
	return this->AuraAbilitySystemComponent;
}

UAuraAttributeSet* UAuraWidgetController::GetAuraAS()
{
	if (this->AuraAttributeSet == nullptr) this->AuraAttributeSet = Cast<UAuraAttributeSet>(this->AttributeSet);
	return this->AuraAttributeSet;
}
