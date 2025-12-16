#include "UI/ViewModel/MVVM_LoadMenu.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"
#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Game/LoadMenuSaveGame.h"

UMVVM_LoadSlot* UMVVM_LoadMenu::GetLoadSLotViewModelByIndex(int32 Index) const
{
	return this->LoadSlots.FindChecked(Index);
}

void UMVVM_LoadMenu::NewSlotButtonPressed(int32 Slot, const FString& EnterName)
{
	AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	this->LoadSlots[Slot]->SetPlayerName(EnterName);
	this->LoadSlots[Slot]->SetMapName(GameMode->StartingMapName);
	this->LoadSlots[Slot]->SlotStatus = ESaveSlotStatus::Taken;

	GameMode->SaveSlotData(this->LoadSlots[Slot], Slot);
	this->LoadSlots[Slot]->InitializeSlot();
}

void UMVVM_LoadMenu::NewGameButtonPressed(int32 Slot)
{
	this->LoadSlots[Slot]->SetWidgetSwitcherIndex.Broadcast(1);
}

void UMVVM_LoadMenu::SelectSlotButtonPressed(int32 Slot)
{
	this->SlotSelected.Broadcast();

	for (const TTuple<int32, UMVVM_LoadSlot*> LoadSlot : this->LoadSlots)
	{
		if (LoadSlot.Key == Slot)
		{
			LoadSlot.Value->EnableSelectSlotButton.Broadcast(false);
		}
		else
		{
			LoadSlot.Value->EnableSelectSlotButton.Broadcast(true);
		}
	}
	this->SelectedSlot = this->LoadSlots[Slot];
}

void UMVVM_LoadMenu::DeleteButtonPressed()
{
	if (IsValid(this->SelectedSlot))
	{
		AAuraGameModeBase::DeleteSlot(this->SelectedSlot->GetLoadSlotName(), this->SelectedSlot->GetSlotIndex());
		this->SelectedSlot->SlotStatus = ESaveSlotStatus::Vacant;
		this->SelectedSlot->InitializeSlot();

		this->SelectedSlot->EnableSelectSlotButton.Broadcast(true);
	}
}

void UMVVM_LoadMenu::PlayButtonPressed()
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	if(IsValid(this->SelectedSlot)) AuraGameMode->TravelToMap(this->SelectedSlot);
}

void UMVVM_LoadMenu::InitializeLoadSlots()
{
	this->LoadSlot_0 = NewObject<UMVVM_LoadSlot>(this, this->LoadSlotViewModelClass);
	this->LoadSlot_0->SetLoadSlotName(FString("LoadSlot_0")); // Filename
	this->LoadSlot_0->SetSlotIndex(0);
	this->LoadSlots.Add(0, this->LoadSlot_0);

	this->LoadSlot_1 = NewObject<UMVVM_LoadSlot>(this, this->LoadSlotViewModelClass);
	this->LoadSlot_1->SetLoadSlotName(FString("LoadSlot_1"));
	this->LoadSlot_1->SetSlotIndex(1);
	this->LoadSlots.Add(1, this->LoadSlot_1);

	this->LoadSlot_2 = NewObject<UMVVM_LoadSlot>(this, this->LoadSlotViewModelClass);
	this->LoadSlot_2->SetLoadSlotName(FString("LoadSlot_2"));
	this->LoadSlot_2->SetSlotIndex(2);
	this->LoadSlots.Add(2, this->LoadSlot_2);
}

void UMVVM_LoadMenu::LoadData()
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	for (const TTuple<int32, UMVVM_LoadSlot*> LoadSlot : this->LoadSlots)
	{
		ULoadMenuSaveGame* SaveObject = AuraGameMode->GetSaveSlotData(LoadSlot.Value->GetLoadSlotName(), LoadSlot.Key);

		LoadSlot.Value->SetPlayerName(SaveObject->PlayerName);
		LoadSlot.Value->SetMapName(SaveObject->MapName);
		LoadSlot.Value->SlotStatus = SaveObject->SaveSlotStatus;
		LoadSlot.Value->InitializeSlot();
	}
}
