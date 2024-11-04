#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "AttributeMenuWidgetController.generated.h"

struct FAuraAttributeInfo;
struct FGameplayAttribute;
struct FGameplayTag;
class UAttributeInfo;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeInfoSignature, const FAuraAttributeInfo&, Info);

UCLASS(BlueprintType, Blueprintable)
class AURA_API UAttributeMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes") FAttributeInfoSignature AttributeInfoDelegate;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes") FOnPlayerStatChangedSignature AttributePointsDelegate;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes") FOnPlayerStatChangedSignature SpellPointsDelegate;

	UFUNCTION(BlueprintCallable) void UpgradeAttribute(const FGameplayTag& AttributeTag);

	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

protected:
	UPROPERTY(EditDefaultsOnly) TObjectPtr<UAttributeInfo> AttributeInfo;

private:
	void BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const;
};
