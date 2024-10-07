#include "AbilitySystem/AsyncTasks/WaitCooldownChange.h"
#include "AbilitySystemComponent.h"

UWaitCooldownChange* UWaitCooldownChange::WaitForCooldownChange(UAbilitySystemComponent* ASC, const FGameplayTag& InCooldownTag)
{
	UWaitCooldownChange* WaitCooldownChange = NewObject<UWaitCooldownChange>();
	WaitCooldownChange->ASC = ASC;
	WaitCooldownChange->CooldownTag = InCooldownTag;

	if (!IsValid(ASC) || !InCooldownTag.IsValid())
	{
		WaitCooldownChange->EndTask();
		return nullptr;
	}

	// to know when a Cooldown effect has started
	ASC->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(WaitCooldownChange, &UWaitCooldownChange::OnActiveEffectAdded);
	// to know when a Colldown effect has ended
	ASC->RegisterGameplayTagEvent(InCooldownTag, EGameplayTagEventType::NewOrRemoved).AddUObject(WaitCooldownChange, &UWaitCooldownChange::CooldownTagChanged);

	return WaitCooldownChange;
}

void UWaitCooldownChange::EndTask()
{
	if (!IsValid(this->ASC)) return;

	this->ASC->RegisterGameplayTagEvent(this->CooldownTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);

	SetReadyToDestroy();
	MarkAsGarbage();
}

void UWaitCooldownChange::CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount)
{
	if (NewCount == 0)
	{
		this->CooldownEnd.Broadcast(0.f);
	}
}

void UWaitCooldownChange::OnActiveEffectAdded(UAbilitySystemComponent* TargetASC, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer AssetTags;
	FGameplayTagContainer GrantedTags;

	SpecApplied.GetAllAssetTags(AssetTags);
	SpecApplied.GetAllGrantedTags(GrantedTags);

	if (AssetTags.HasTagExact(this->CooldownTag) || GrantedTags.HasTagExact(this->CooldownTag))
	{
		FGameplayEffectQuery GameplayEffectQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(this->CooldownTag.GetSingleTagContainer());
		TArray<float> TimesRemaining = this->ASC->GetActiveEffectsTimeRemaining(GameplayEffectQuery);
		if (TimesRemaining.Num() > 0)
		{
			float TimeRemaining = TimesRemaining[0];
			for (int32 i = 0; i < TimesRemaining.Num(); i++)
			{
				if (TimesRemaining[i] > TimeRemaining)
				{
					TimeRemaining = TimesRemaining[i];
				}
			}
			this->CooldownStart.Broadcast(TimeRemaining);
		}
	}
}
