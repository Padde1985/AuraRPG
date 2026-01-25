#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AuraGameModeBase.generated.h"

class UCharacterClassInfo;
class UAbilityInfo;
class UMVVM_LoadSlot;
class USaveGame;
class ULoadMenuSaveGame;
class ULootTiers;

UCLASS()
class AURA_API AAuraGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults") TObjectPtr<UCharacterClassInfo> CharacterClassInfo;
	UPROPERTY(EditDefaultsOnly, Category = "Ability Info") TObjectPtr<UAbilityInfo> AbilityInfo;
	UPROPERTY(EditDefaultsOnly, Category = "Loot Tiers") TObjectPtr<ULootTiers> LootTiers;
	UPROPERTY(EditDefaultsOnly) TSubclassOf<USaveGame> LoadMenuSaveGameClass;
	UPROPERTY(EditDefaultsOnly) FString StartingMapName;
	UPROPERTY(EditDefaultsOnly) TSoftObjectPtr<UWorld> DefaultMap;
	UPROPERTY(EditDefaultsOnly) TMap<FString, TSoftObjectPtr<UWorld>> Maps;
	UPROPERTY(EditDefaultsOnly) FName DefaultPlayerStartTag;

	void SaveSlotData(UMVVM_LoadSlot* LoadSlot, int32 SlotIndex) const;
	ULoadMenuSaveGame* GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const;
	static void DeleteSlot(const FString& SlotName, int32 SlotIndex);
	void TravelToMap(const UMVVM_LoadSlot* Slot);
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	ULoadMenuSaveGame* RetrieveInGameSaveData();
	void SaveInGameProgressData(ULoadMenuSaveGame* SaveObject) const;
	void SaveWorldState(UWorld* World, const FString& DestinationMapAssetName = FString()) const;
	void LoadWorldState(UWorld* World) const;
	FString GetMapNameByMapAssetName(const FString& MapAssetName) const;
	void PlayerDied(const ACharacter* DeadCharacter);

protected:
	virtual void BeginPlay() override;
};
