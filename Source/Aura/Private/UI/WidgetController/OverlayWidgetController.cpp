#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"
#include "AbilitySystem/Data/LevelInfo.h"

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
	AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);

	AuraPlayerState->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnXPChanged);
	AuraPlayerState->OnLevelChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnLevelChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Set->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)	{ this->OnHealthChanged.Broadcast(Data.NewValue); });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Set->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)	{ this->OnMaxHealthChanged.Broadcast(Data.NewValue); });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Set->GetManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)	{ this->OnManaChanged.Broadcast(Data.NewValue); });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Set->GetMaxManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)	{ this->OnMaxManaChanged.Broadcast(Data.NewValue); });
	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		AuraASC->EffectAssetTags.AddLambda(
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
		if (AuraASC->bStartupAbilitiesGiven)
		{
			this->OnInitializeStartupAbilities(AuraASC);
		}
		// otherwise just bind to callback and wait for Abilities to be set
		else
		{
			AuraASC->AbilitiesGiven.AddUObject(this, &UOverlayWidgetController::OnInitializeStartupAbilities);
		}
	}
}

void UOverlayWidgetController::OnInitializeStartupAbilities(UAuraAbilitySystemComponent* ASC)
{
	// TODO: Get information about all given abilities, look up ABilityInfo, broadcast to widgets
	if (!ASC->bStartupAbilitiesGiven) return;

	FForeachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda(
		[this](const FGameplayAbilitySpec& Spec)
	{
		// TODO: need a way to figure out the tag for a given spec
		FAuraAbilityInfo Info = this->AbilityInfo->FindAbilityInfoForTag(UAuraAbilitySystemComponent::GetAbilityTagFromSpec(Spec));
		Info.InputTag = UAuraAbilitySystemComponent::GetInputTagFromSpec(Spec);
		this->AbilityInfoDelegate.Broadcast(Info);
	}
	);
	ASC->ForEachAbility(BroadcastDelegate);
}

void UOverlayWidgetController::OnXPChanged(int32 NewXP) const
{
	const AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	const ULevelInfo* LevelInfo = AuraPlayerState->LevelInfo;

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
