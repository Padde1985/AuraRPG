#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "GameplayTagContainer.h"
#include "OverlayWidgetController.generated.h"

class UAuraUserWidget;
class UAbilityInfo;
class UAuraAbilitySystemComponent;
struct FOnAttributeChangeData;

USTRUCT(BlueprintType) struct FUIWidgetRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTag MessageTag = FGameplayTag();
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FText Message = FText();
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TSubclassOf<UAuraUserWidget> MessageWidget;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) UTexture2D* Image = nullptr;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageRowWidgetSignature, FUIWidgetRow, Row);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature, float, NewValue);

UCLASS(BlueprintType, Blueprintable)
class AURA_API UOverlayWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable, category = "GAS|Attributes") FOnAttributeChangedSignature OnHealthChanged;
	UPROPERTY(BlueprintAssignable, category = "GAS|Attributes") FOnAttributeChangedSignature OnMaxHealthChanged;
	UPROPERTY(BlueprintAssignable, category = "GAS|Attributes") FOnAttributeChangedSignature OnManaChanged;
	UPROPERTY(BlueprintAssignable, category = "GAS|Attributes") FOnAttributeChangedSignature OnMaxManaChanged;
	UPROPERTY(BlueprintAssignable, category = "GAS|XP") FOnAttributeChangedSignature OnXPPercentChanged;
	UPROPERTY(BlueprintAssignable, category = "GAS|Messages") FMessageRowWidgetSignature MessageWidgetRowDelegate;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Level") FOnPlayerStatChangedSignature OnPlayerLevelChangedDelegate;

	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data", meta = (AllowPrivateAccess = "true")) TObjectPtr<UDataTable> MessageWidgetDataTable;

	template<typename T> T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag);
	void OnXPChanged(int32 NewXP);
	void OnLevelChanged(int32 NewLevel);
	void OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot) const;
};

// Template functions are automatically craeted in the header file
template<typename T>
inline T* UOverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{
	return DataTable->FindRow<T>(Tag.GetTagName(), TEXT(""));
}
