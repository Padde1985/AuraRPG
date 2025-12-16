#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AuraGameModeBase.generated.h"

class UCharacterClassInfo;
class UAbilityInfo;
class UMVVM_LoadSlot;
class USaveGame;
class ULoadMenuSaveGame;

UCLASS()
class AURA_API AAuraGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults") TObjectPtr<UCharacterClassInfo> CharacterClassInfo;
	UPROPERTY(EditDefaultsOnly, Category = "Ability Info") TObjectPtr<UAbilityInfo> AbilityInfo;
	UPROPERTY(EditDefaultsOnly) TSubclassOf<USaveGame> LoadMenuSaveGameClass;
	UPROPERTY(EditDefaultsOnly) FString StartingMapName;
	UPROPERTY(EditDefaultsOnly) TSoftObjectPtr<UWorld> DefaultMap;
	UPROPERTY(EditDefaultsOnly) TMap<FString, TSoftObjectPtr<UWorld>> Maps;

	void SaveSlotData(UMVVM_LoadSlot* LoadSlot, int32 SlotIndex);
	ULoadMenuSaveGame* GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const;
	static void DeleteSlot(const FString& SlotName, int32 SlotIndex);
	void TravelToMap(UMVVM_LoadSlot* Slot);

protected:
	virtual void BeginPlay() override;
};
