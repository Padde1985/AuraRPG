#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AuraWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChangedSignature, int32, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityInfoSignature, const FAuraAbilityInfo&, Info);

class APlayerController;
class APlayerState;
class UAbilitySystemComponent;
class UAttributeSet;
class AAuraPlayerController;
class AAuraPlayerState;
class UAuraAbilitySystemComponent;
class UAuraAttributeSet;
class UAbilityInfo;

USTRUCT(BlueprintType) struct FWidgetControllerParams
{
	GENERATED_BODY()

	FWidgetControllerParams() {}
	FWidgetControllerParams(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS) 
		: PlayerController(PC), PlayerState(PS), AbilitySystemComponent(ASC), AttributeSet(AS) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite) TObjectPtr<APlayerController> PlayerController = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TObjectPtr<APlayerState> PlayerState = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TObjectPtr<UAttributeSet> AttributeSet = nullptr;
};

UCLASS()
class AURA_API UAuraWidgetController : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable, Category = "GAS|Messages") FAbilityInfoSignature AbilityInfoDelegate;

	UFUNCTION(BlueprintCallable) void SetWidgetControllerParams(const FWidgetControllerParams& WCParams);
	UFUNCTION(BlueprintCallable) virtual void BroadcastInitialValues();
	virtual void BindCallbacksToDependencies();
	void BroadcastAbilityInfo();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget Controller", meta = (AllowPrivateAccess = "true")) TObjectPtr<APlayerController> PlayerController;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget Controller", meta = (AllowPrivateAccess = "true")) TObjectPtr<APlayerState> PlayerState;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget Controller", meta = (AllowPrivateAccess = "true")) TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget Controller", meta = (AllowPrivateAccess = "true")) TObjectPtr<UAttributeSet> AttributeSet;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget Controller", meta = (AllowPrivateAccess = "true")) TObjectPtr<AAuraPlayerController> AuraPlayerController;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget Controller", meta = (AllowPrivateAccess = "true")) TObjectPtr<AAuraPlayerState> AuraPlayerState;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget Controller", meta = (AllowPrivateAccess = "true")) TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget Controller", meta = (AllowPrivateAccess = "true")) TObjectPtr<UAuraAttributeSet> AuraAttributeSet;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data", meta = (AllowPrivateAccess = "true")) TObjectPtr<UAbilityInfo> AbilityInfo;

	AAuraPlayerController* GetAuraPC();
	AAuraPlayerState* GetAuraPS();
	UAuraAbilitySystemComponent* GetAuraASC();
	UAuraAttributeSet* GetAuraAS();
};
