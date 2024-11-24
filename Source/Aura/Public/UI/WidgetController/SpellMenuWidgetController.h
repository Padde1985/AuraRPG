#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "GameplayTagContainer.h"
#include "AuraGameplayTags.h"
#include "SpellMenuWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FSpellGlobeSelectedSignature, bool, bEnableSpellPointsButton, bool, bEnableEquipButton, FString, DescriptionString, FString, NextLevelDescriptionString);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitForEquippedSelectionSignature, const FGameplayTag&, AbilityType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpellGlobeReassignedSignature, const FGameplayTag&, AbilityType);

struct FSelectedAbility
{
	FGameplayTag Ability = FGameplayTag();
	FGameplayTag Status = FGameplayTag();
};

UCLASS(Blueprintable, BlueprintType)
class AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable) FOnPlayerStatChangedSignature SpellPointsChanged;
	UPROPERTY(BlueprintAssignable) FSpellGlobeSelectedSignature SpellGlobeSelectedDelegate;
	UPROPERTY(BlueprintAssignable) FWaitForEquippedSelectionSignature WaitForEquippedDelegate;
	UPROPERTY(BlueprintAssignable) FWaitForEquippedSelectionSignature StopWaitForEquippedDelegate;
	UPROPERTY(BlueprintAssignable) FSpellGlobeReassignedSignature SpellGlobeReassignedDelegate;

	UFUNCTION(BlueprintCallable) void SpellGlobeSelected(const FGameplayTag& AbilityTag);
	UFUNCTION(BlueprintCallable) void SpendPointButtonPressed();
	UFUNCTION(BlueprintCallable) void GlobeDeselect();
	UFUNCTION(BlueprintCallable) void EquippedButtonPressed();
	UFUNCTION(BlueprintCallable) void SpellGlobeRowPressed(const FGameplayTag& SlotTag, const FGameplayTag& AbilityType);
	void OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot);

	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

private:
	FSelectedAbility SelectedAbility = { FAuraGameplayTags::Get().Abilities_None, FAuraGameplayTags::Get().Abilities_Status_Locked };
	int32 CurrentSP = 0;
	bool bWaitingForEquippedSelection = false;
	FGameplayTag SelectedSlot;

	static void EnableButtons(const FGameplayTag& Status, int32 SP, bool& bShouldEnableSPButton, bool& bShouldEnableEquipButton);
};
