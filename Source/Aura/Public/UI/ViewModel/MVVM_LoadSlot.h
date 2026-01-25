#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Game/LoadMenuSaveGame.h"
#include "MVVM_LoadSlot.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetWidgetSwitcherIndex, int32, WidgetSwitcherIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnableSelectSlotButton, bool, bEnable);

UCLASS()
class AURA_API UMVVM_LoadSlot : public UMVVMViewModelBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable) FSetWidgetSwitcherIndex SetWidgetSwitcherIndex;
	UPROPERTY(BlueprintAssignable) FEnableSelectSlotButton EnableSelectSlotButton;
	UPROPERTY() TEnumAsByte<ESaveSlotStatus> SlotStatus;
	UPROPERTY() FName PlayerStartTag;
	UPROPERTY() FString MapAssetName;

	void InitializeSlot() const;
	void SetLoadSlotName(FString inLoadSlotName);
	FString GetLoadSlotName() const;
	void SetPlayerName(FString InPlayerName);
	FString GetPlayerName() const;
	void SetSlotIndex(int32 InSlotIndex);
	int32 GetSlotIndex() const;
	void SetMapName(FString InMapName);
	FString GetMapName() const;
	void SetPlayerLevel(int32 InLevel);
	int32 GetPlayerLevel() const;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta = (AllowPrivateAccess = "true")) FString LoadSlotName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta = (AllowPrivateAccess = "true")) FString PlayerName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta = (AllowPrivateAccess = "true")) FString MapName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta = (AllowPrivateAccess = "true")) int32 SlotIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta = (AllowPrivateAccess = "true")) int32 PlayerLevel;
};
