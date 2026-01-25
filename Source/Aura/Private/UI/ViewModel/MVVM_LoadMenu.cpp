#include "UI/ViewModel/MVVM_LoadMenu.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"
#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Game/LoadMenuSaveGame.h"
#include "Game/AuraGameInstance.h"

// get load slot view model by user selected index (0-2)
UMVVM_LoadSlot* UMVVM_LoadMenu::GetLoadSlotViewModelByIndex(int32 Index) const
{
	return this->LoadSlots.FindChecked(Index);
}

// creating a new slot where all data will be saved to
void UMVVM_LoadMenu::NewSlotButtonPressed(int32 Slot, const FString& EnterName)
{
	AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (!IsValid(GameMode))
	{
		GEngine->AddOnScreenDebugMessage(1, 15.f, FColor::Magenta, FString("Please switch to Single player game"));
		return;
	}

	this->LoadSlots[Slot]->SetPlayerName(EnterName);
	this->LoadSlots[Slot]->SetMapName(GameMode->StartingMapName);
	this->LoadSlots[Slot]->SetPlayerLevel(1);
	this->LoadSlots[Slot]->SlotStatus = ESaveSlotStatus::Taken;
	this->LoadSlots[Slot]->PlayerStartTag = GameMode->DefaultPlayerStartTag;
	this->LoadSlots[Slot]->MapAssetName = GameMode->DefaultMap.ToSoftObjectPath().GetAssetName();

	GameMode->SaveSlotData(this->LoadSlots[Slot], Slot);
	this->LoadSlots[Slot]->InitializeSlot();

	UAuraGameInstance* GameInstance = Cast<UAuraGameInstance>(GameMode->GetGameInstance());
	GameInstance->LoadSlotName = this->LoadSlots[Slot]->GetLoadSlotName();
	GameInstance->LoadSlotIndex = Slot;
	GameInstance->PlayerStartTag = GameMode->DefaultPlayerStartTag;
}

// create a new game with the selected slot
void UMVVM_LoadMenu::NewGameButtonPressed(int32 Slot)
{
	this->LoadSlots[Slot]->SetWidgetSwitcherIndex.Broadcast(1);
}

// select a slot to enable starting the game, delete the slot or just quit (going back to Load Menu)
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

// delete a given slot and all data associated within (slots are stored as a file in the game folder under /saved/savegames
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

// start the game of selected slot and load saved data if available
void UMVVM_LoadMenu::PlayButtonPressed()
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	UAuraGameInstance* GameInstance = Cast<UAuraGameInstance>(AuraGameMode->GetGameInstance());
	GameInstance->PlayerStartTag = this->SelectedSlot->PlayerStartTag;
	GameInstance->LoadSlotName = this->SelectedSlot->GetLoadSlotName();
	GameInstance->LoadSlotIndex = this->SelectedSlot->GetSlotIndex();

	if(IsValid(this->SelectedSlot)) AuraGameMode->TravelToMap(this->SelectedSlot);
}

// initialize the Load slots when landing on the game screen
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

// load central data from the savegame and show in the widget
void UMVVM_LoadMenu::LoadData()
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	// since this is a single player game, the client does not have a game mode
	if (!IsValid(AuraGameMode)) return;

	for (const TTuple<int32, UMVVM_LoadSlot*> LoadSlot : this->LoadSlots)
	{
		ULoadMenuSaveGame* SaveObject = AuraGameMode->GetSaveSlotData(LoadSlot.Value->GetLoadSlotName(), LoadSlot.Key);

		LoadSlot.Value->SetPlayerName(SaveObject->PlayerName);
		LoadSlot.Value->SetMapName(SaveObject->MapName);
		LoadSlot.Value->SlotStatus = SaveObject->SaveSlotStatus;
		LoadSlot.Value->PlayerStartTag = SaveObject->PlayerStartTag;
		LoadSlot.Value->SetPlayerLevel(SaveObject->PlayerLevel);
		LoadSlot.Value->InitializeSlot();
	}
}
