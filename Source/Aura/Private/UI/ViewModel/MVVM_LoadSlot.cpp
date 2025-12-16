#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadSlot::InitializeSlot()
{
	this->SetWidgetSwitcherIndex.Broadcast(this->SlotStatus.GetIntValue());
}

void UMVVM_LoadSlot::SetLoadSlotName(FString inLoadSlotName)
{
	UE_MVVM_SET_PROPERTY_VALUE(LoadSlotName, inLoadSlotName);
}

FString UMVVM_LoadSlot::GetLoadSlotName() const
{
	return this->LoadSlotName;
}

void UMVVM_LoadSlot::SetPlayerName(FString InPlayerName)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerName, InPlayerName);
}

FString UMVVM_LoadSlot::GetPlayerName() const
{
	return this->PlayerName;
}

void UMVVM_LoadSlot::SetSlotIndex(int32 InSlotIndex)
{
	UE_MVVM_SET_PROPERTY_VALUE(SlotIndex, InSlotIndex);
}

int32 UMVVM_LoadSlot::GetSlotIndex() const
{
	return this->SlotIndex;
}

void UMVVM_LoadSlot::SetMapName(FString InMapName)
{
	UE_MVVM_SET_PROPERTY_VALUE(MapName, InMapName);
}

FString UMVVM_LoadSlot::GetMapName() const
{
	return this->MapName;
}
