#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "DamageTextComponent.generated.h"

UCLASS()
class AURA_API UDamageTextComponent : public UWidgetComponent
{
	GENERATED_BODY()
	
public:
	// event used in blueprint, no implementation in C++
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable) void SetDamageText(float Damage, bool bIsBlocked, bool bIsCritical);
};
