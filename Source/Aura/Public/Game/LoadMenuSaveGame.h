#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GameplayTagContainer.h"
#include "LoadMenuSaveGame.generated.h"

class UGameplayAbility;

UENUM(BlueprintType)
enum ESaveSlotStatus
{
	Vacant,
	EnterName,
	Taken
};

USTRUCT()
struct FSavedActor
{
	GENERATED_BODY()

	UPROPERTY() FName ActorName = FName();
	UPROPERTY() FTransform Transform = FTransform();
	UPROPERTY() TArray<uint8> Bytes; // serialized variables from the Actor with a special specifier
};

inline bool operator==(const FSavedActor& Left, const FSavedActor& Right)
{
	return Left.ActorName == Right.ActorName;
}

USTRUCT()
struct FSavedMap
{
	GENERATED_BODY()

	UPROPERTY() FString MapAssetName = FString();
	UPROPERTY() TArray<FSavedActor> SavedActors;
};

USTRUCT(BlueprintType)
struct FSavedAbility
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Class defaults") TSubclassOf<UGameplayAbility> GameplayAbility;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Class defaults") FGameplayTag AbilityTag = FGameplayTag();
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Class defaults") FGameplayTag AbilityStatus = FGameplayTag();
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Class defaults") FGameplayTag AbilitySlot = FGameplayTag();
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Class defaults") FGameplayTag AbilityType = FGameplayTag();
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Class defaults") int32 Level;
};

// overload == operator, otherwise AddUnique throws an error
inline bool operator==(const FSavedAbility& Left, const FSavedAbility& Right)
{
	return Left.AbilityTag.MatchesTagExact(Right.AbilityTag);
}

UCLASS()
class AURA_API ULoadMenuSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY() FString SlotName = FString();
	UPROPERTY() int32 SlotIndex = 0;
	UPROPERTY() FString PlayerName = FString("Default Name");
	UPROPERTY() FString MapName = FString("Default MapName");
	UPROPERTY() FString MapAssetName = FString();
	UPROPERTY() TEnumAsByte<ESaveSlotStatus> SaveSlotStatus = Vacant;
	UPROPERTY() FName PlayerStartTag;
	UPROPERTY() int32 PlayerLevel = 1;
	UPROPERTY() int32 XP = 0;
	UPROPERTY() int32 SpellPoints = 0;
	UPROPERTY() int32 AttributePoints = 0;
	UPROPERTY() float Strength = 0.f;
	UPROPERTY() float Intelligence = 0.f;
	UPROPERTY() float Resilience = 0.f;
	UPROPERTY() float Vigor = 0.f;
	UPROPERTY() bool bFirstTimeLoadIn = true;
	UPROPERTY() TArray<FSavedAbility> SavedAbilities;
	UPROPERTY() TArray<FSavedMap> SavedMaps;

	FSavedMap GetSavedMapByMapName(const FString& InMapName) const;
	bool HasMap(const FString& InMapName);
};
