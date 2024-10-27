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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageRowWidgetSignature, FUIWidgetRow, Row);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityInfoSignature, const FAuraAbilityInfo&, Info);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChangedSignature, int32, NewValue);

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
	UPROPERTY(BlueprintAssignable, Category = "GAS|Messages") FAbilityInfoSignature AbilityInfoDelegate;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Level") FOnPlayerStatChangedSignature OnPlayerLevelChangedDelegate;

	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data", meta = (AllowPrivateAccess = "true")) TObjectPtr<UDataTable> MessageWidgetDataTable;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data", meta = (AllowPrivateAccess = "true")) TObjectPtr<UAbilityInfo> AbilityInfo;


	template<typename T> T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag);
	void OnInitializeStartupAbilities(UAuraAbilitySystemComponent* ASC);
	void OnXPChanged(int32 NewXP) const;
	void OnLevelChanged(int32 NewLevel);
};

// Template functions are automatically craeted in the header file
template<typename T>
inline T* UOverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{
	return DataTable->FindRow<T>(Tag.GetTagName(), TEXT(""));
}
