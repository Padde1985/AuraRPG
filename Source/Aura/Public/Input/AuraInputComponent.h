#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "AuraInputConfig.h"
#include "AuraInputComponent.generated.h"

UCLASS()
class AURA_API UAuraInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
	
public:
	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType> void BindAbilityActions(const UAuraInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc);
};

template<class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
inline void UAuraInputComponent::BindAbilityActions(const UAuraInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc)
{
	check(InputConfig);

	for (const TPair<FGameplayTag, TObjectPtr<UInputAction>>& Pair : InputConfig->AbilityInputActions)
	{
		if (Pair.Value && Pair.Key.IsValid())
		{
			if (PressedFunc) BindAction(Pair.Value, ETriggerEvent::Started, Object, PressedFunc, Pair.Key);
			if (ReleasedFunc) BindAction(Pair.Value, ETriggerEvent::Completed, Object, ReleasedFunc, Pair.Key);
			if (HeldFunc) BindAction(Pair.Value, ETriggerEvent::Triggered, Object, HeldFunc, Pair.Key);
		}
	}
}
