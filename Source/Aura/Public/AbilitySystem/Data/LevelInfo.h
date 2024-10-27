#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelInfo.generated.h"

USTRUCT(BlueprintType)
struct FAuraLevelInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) int32 Level;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) int32 LevelUpRequirement;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) int32 AttributePoints;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) int32 SpellPoints;
};

UCLASS()
class AURA_API ULevelInfo : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LevelInformation") TArray<FAuraLevelInfo> LevelInformation;

	int32 FindLevelForXP(int32 XP) const;
	FAuraLevelInfo GetInfoForLevel(int32 Level) const;
};
