#include "Input/AuraInputConfig.h"

const UInputAction* UAuraInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	const UInputAction* Action = this->AbilityInputActions.Find(InputTag)->Get();
	if (Action != nullptr) return Action;

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("AbilitInputAction not found for InputTag {%s}"), *InputTag.ToString(), *GetNameSafe(this));
	}
	return nullptr;
}
