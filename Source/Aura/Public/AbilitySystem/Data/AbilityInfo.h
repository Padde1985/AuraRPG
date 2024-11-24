#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "AbilityInfo.generated.h"

class UGameplayAbility;

USTRUCT(BlueprintType)
struct FAuraAbilityInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FGameplayTag AbilityTag = FGameplayTag();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TObjectPtr<const UTexture2D> Icon = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TObjectPtr<const UMaterialInterface> BackgroundMaterial = nullptr;
	UPROPERTY(BlueprintReadOnly) FGameplayTag InputTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly) FGameplayTag CooldownTag;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly) FGameplayTag AbilityType = FGameplayTag();
	UPROPERTY(BlueprintReadOnly) FGameplayTag StatusTag = FGameplayTag();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) int32 LevelRequirement = 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TSubclassOf<UGameplayAbility> Ability;

};

UCLASS()
class AURA_API UAbilityInfo : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AbilityInformation") TArray<FAuraAbilityInfo> AbilityInformation;

	UFUNCTION() FAuraAbilityInfo FindAbilityInfoForTag(const FGameplayTag& AbilityTag, bool bLogNotFound = false) const;
};
