#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"
#include "Game/LoadMenuSaveGame.h"
#include "GameFramework/PlayerStart.h"
#include "Game/AuraGameInstance.h"
#include "EngineUtils.h"
#include "Interaction/SaveInterface.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "AuraLogChannels.h"
#include "GameFramework/Character.h"

// save initial game data when slot is being created
void AAuraGameModeBase::SaveSlotData(UMVVM_LoadSlot* LoadSlot, int32 SlotIndex) const
{
	if (UGameplayStatics::DoesSaveGameExist(LoadSlot->GetLoadSlotName(), SlotIndex)) UGameplayStatics::DeleteGameInSlot(LoadSlot->GetLoadSlotName(), SlotIndex);

	USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(this->LoadMenuSaveGameClass);
	ULoadMenuSaveGame* LoadMenuSaveGame = Cast<ULoadMenuSaveGame>(SaveGameObject);
	LoadMenuSaveGame->PlayerName = LoadSlot->GetPlayerName();
	LoadMenuSaveGame->MapName = LoadSlot->GetMapName();
	LoadMenuSaveGame->MapAssetName = LoadSlot->MapAssetName;
	LoadMenuSaveGame->SaveSlotStatus = ESaveSlotStatus::Taken;
	LoadMenuSaveGame->PlayerStartTag = LoadSlot->PlayerStartTag;

	UGameplayStatics::SaveGameToSlot(LoadMenuSaveGame, LoadSlot->GetLoadSlotName(), SlotIndex);
}

// load data from slot for given slot index (slot name can be idsentical for multiple slots)
ULoadMenuSaveGame* AAuraGameModeBase::GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const
{
	USaveGame* SaveGameObject = nullptr;

	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		SaveGameObject = UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex);
	}
	else
	{
		SaveGameObject = UGameplayStatics::CreateSaveGameObject(this->LoadMenuSaveGameClass);
	}

	return Cast<ULoadMenuSaveGame>(SaveGameObject);
}

// delete unwanted slot and erase all data for it
void AAuraGameModeBase::DeleteSlot(const FString& SlotName, int32 SlotIndex)
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex)) UGameplayStatics::DeleteGameInSlot(SlotName, SlotIndex);
}

// travel to specifed map (either default map or map from saved game data)
void AAuraGameModeBase::TravelToMap(const UMVVM_LoadSlot* Slot)
{
	UGameplayStatics::OpenLevelBySoftObjectPtr(Slot, this->Maps.FindChecked(Slot->GetMapName()));
}

// choose player start when loading map (save game location, map entrance or default start)
AActor* AAuraGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	UAuraGameInstance* GameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), Actors);
	if (Actors.Num() > 0)
	{
		AActor* SelectedActor = Actors[0];
		for (AActor* Actor : Actors)
		{
			if (APlayerStart* PlayerStart = Cast<APlayerStart>(Actor))
			{
				if (PlayerStart->PlayerStartTag == GameInstance->PlayerStartTag)
				{
					SelectedActor = PlayerStart;
					return SelectedActor;
				}
			}
		}
	}
	return nullptr;
}

// get data for slot widget
ULoadMenuSaveGame* AAuraGameModeBase::RetrieveInGameSaveData()
{
	UAuraGameInstance* GameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	const FString InGameLoadSlotName = GameInstance->LoadSlotName;
	const int32 InGameLoadSlotIndex = GameInstance->LoadSlotIndex;

	return this->GetSaveSlotData(InGameLoadSlotName, InGameLoadSlotIndex);
}

// update save game data before saving to disk
void AAuraGameModeBase::SaveInGameProgressData(ULoadMenuSaveGame* SaveObject) const
{
	UAuraGameInstance* GameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	const FString InGameLoadSlotName = GameInstance->LoadSlotName;
	const int32 InGameLoadSlotIndex = GameInstance->LoadSlotIndex;
	GameInstance->PlayerStartTag = SaveObject->PlayerStartTag;

	UGameplayStatics::SaveGameToSlot(SaveObject, InGameLoadSlotName, InGameLoadSlotIndex);
}

// save world status (map name, actors in the world, etc.)
void AAuraGameModeBase::SaveWorldState(UWorld* World, const FString& DestinationMapAssetName) const
{
	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);

	UAuraGameInstance* GI = Cast<UAuraGameInstance>(GetGameInstance());
	check(GI);

	if (ULoadMenuSaveGame* SaveGame = this->GetSaveSlotData(GI->LoadSlotName, GI->LoadSlotIndex))
	{
		if (DestinationMapAssetName != FString())
		{
			SaveGame->MapAssetName = DestinationMapAssetName;
			SaveGame->MapName = this->GetMapNameByMapAssetName(DestinationMapAssetName);
		}

		if (!SaveGame->HasMap(WorldName))
		{
			FSavedMap NewSavedMap;
			NewSavedMap.MapAssetName = WorldName;
			SaveGame->SavedMaps.Add(NewSavedMap);
		}

		FSavedMap SavedMap = SaveGame->GetSavedMapByMapName(WorldName);
		SavedMap.SavedActors.Empty();

		// loop over all Actors in the world (could be tuned by storing array of relevant actors in GameInstance
		for (FActorIterator It(World); It; ++It)
		{
			AActor* Actor = *It;
			if (!IsValid(Actor) || !Actor->Implements<USaveInterface>()) continue;

			FSavedActor SavedActor;
			SavedActor.ActorName = Actor->GetFName();
			SavedActor.Transform = Actor->GetTransform();

			FMemoryWriter MemoryWriter(SavedActor.Bytes);
			FObjectAndNameAsStringProxyArchive ProxyArchive(MemoryWriter, true);
			ProxyArchive.ArIsSaveGame = true;

			Actor->Serialize(ProxyArchive);
			SavedMap.SavedActors.AddUnique(SavedActor);
		}

		for (FSavedMap& MapToReplace : SaveGame->SavedMaps)
		{
			if (MapToReplace.MapAssetName == WorldName) MapToReplace = SavedMap;
		}
		UGameplayStatics::SaveGameToSlot(SaveGame, GI->LoadSlotName, GI->LoadSlotIndex);
	}
}

// load world state back from disk
void AAuraGameModeBase::LoadWorldState(UWorld* World) const
{
	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);

	UAuraGameInstance* GI = Cast<UAuraGameInstance>(GetGameInstance());
	check(GI);

	if (UGameplayStatics::DoesSaveGameExist(GI->LoadSlotName, GI->LoadSlotIndex))
	{
		ULoadMenuSaveGame* SaveGame = Cast<ULoadMenuSaveGame>(UGameplayStatics::LoadGameFromSlot(GI->LoadSlotName, GI->LoadSlotIndex));
		if (SaveGame == nullptr)
		{
			UE_LOG(LogAura, Error, TEXT("Failed to load slot"));
			return;
		}

		for (FActorIterator It(World); It; ++It)
		{
			AActor* Actor = *It;
			if (!Actor->Implements<USaveInterface>()) continue;

			for (FSavedActor SaveActor : SaveGame->GetSavedMapByMapName(WorldName).SavedActors)
			{
				if (SaveActor.ActorName == Actor->GetFName())
				{
					if (ISaveInterface::Execute_ShouldLoadTransform(Actor)) Actor->SetActorTransform(SaveActor.Transform);

					FMemoryReader MemoryReader(SaveActor.Bytes);
					FObjectAndNameAsStringProxyArchive ProxyArchive(MemoryReader, true);
					ProxyArchive.ArIsSaveGame = true;
					Actor->Serialize(ProxyArchive); // converts binary back into actual atrributes and values

					ISaveInterface::Execute_LoadActor(Actor);
				}
			}
		}
	}
}

// convert map asset name (technical name) to display name for user
FString AAuraGameModeBase::GetMapNameByMapAssetName(const FString& MapAssetName) const
{
	for (TTuple<FString, TSoftObjectPtr<UWorld>> Map : this->Maps)
	{
		if (Map.Value.ToSoftObjectPath().GetAssetName() == MapAssetName) return Map.Key;
	}

	return FString();
}

// callback to event of player died
void AAuraGameModeBase::PlayerDied(const ACharacter* DeadCharacter)
{
	ULoadMenuSaveGame* SaveGame = this->RetrieveInGameSaveData();
	if (!IsValid(SaveGame)) return;

	UGameplayStatics::OpenLevel(DeadCharacter, FName(SaveGame->MapAssetName));
}

// add starting map to map array
void AAuraGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	this->Maps.Add(this->StartingMapName, this->DefaultMap);
}