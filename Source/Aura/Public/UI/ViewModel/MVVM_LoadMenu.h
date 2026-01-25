#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVM_LoadMenu.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSlotSelected);

class UMVVM_LoadSlot;

UCLASS()
class AURA_API UMVVM_LoadMenu : public UMVVMViewModelBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly) TSubclassOf<UMVVM_LoadSlot> LoadSlotViewModelClass;
	UPROPERTY(BlueprintAssignable) FSlotSelected SlotSelected;

	UFUNCTION(BlueprintPure) UMVVM_LoadSlot* GetLoadSlotViewModelByIndex(int32 Index) const;
	UFUNCTION(BlueprintCallable) void NewSlotButtonPressed(int32 Slot, const FString& EnterName);
	UFUNCTION(BlueprintCallable) void NewGameButtonPressed(int32 Slot);
	UFUNCTION(BlueprintCallable) void SelectSlotButtonPressed(int32 Slot);
	UFUNCTION(BlueprintCallable) void DeleteButtonPressed();
	UFUNCTION(BlueprintCallable) void PlayButtonPressed();
	
	void InitializeLoadSlots();
	void LoadData();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "", meta = (AllowPrivateAccess = "true")) TMap<int32, UMVVM_LoadSlot*> LoadSlots;
	UPROPERTY() TObjectPtr<UMVVM_LoadSlot> LoadSlot_0;
	UPROPERTY() TObjectPtr<UMVVM_LoadSlot> LoadSlot_1;
	UPROPERTY() TObjectPtr<UMVVM_LoadSlot> LoadSlot_2;
	UPROPERTY() UMVVM_LoadSlot* SelectedSlot;
};
