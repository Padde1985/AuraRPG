#include "UI/ViewModel/MVVM_LoadSlot.h"

// broadcast information about a newly created slot
void UMVVM_LoadSlot::InitializeSlot() const
{
	this->SetWidgetSwitcherIndex.Broadcast(this->SlotStatus.GetIntValue());
}

// update the model view viewmodel about a changed value
void UMVVM_LoadSlot::SetLoadSlotName(FString inLoadSlotName)
{
	UE_MVVM_SET_PROPERTY_VALUE(LoadSlotName, inLoadSlotName);
}

FString UMVVM_LoadSlot::GetLoadSlotName() const
{
	return this->LoadSlotName;
}

// update the model view viewmodel about a changed value
void UMVVM_LoadSlot::SetPlayerName(FString InPlayerName)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerName, InPlayerName);
}

FString UMVVM_LoadSlot::GetPlayerName() const
{
	return this->PlayerName;
}

// update the model view viewmodel about a changed value
void UMVVM_LoadSlot::SetSlotIndex(int32 InSlotIndex)
{
	UE_MVVM_SET_PROPERTY_VALUE(SlotIndex, InSlotIndex);
}

int32 UMVVM_LoadSlot::GetSlotIndex() const
{
	return this->SlotIndex;
}

// update the model view viewmodel about a changed value
void UMVVM_LoadSlot::SetMapName(FString InMapName)
{
	UE_MVVM_SET_PROPERTY_VALUE(MapName, InMapName);
}

FString UMVVM_LoadSlot::GetMapName() const
{
	return this->MapName;
}

// update the model view viewmodel about a changed value
void UMVVM_LoadSlot::SetPlayerLevel(int32 InLevel)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerLevel, InLevel);
}

int32 UMVVM_LoadSlot::GetPlayerLevel() const
{
	return this->PlayerLevel;
}