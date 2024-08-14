#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterClassInfo.generated.h"

class UGameplayEffect;
class UGameplayAbility;
class UCurveTable;

UENUM(BlueprintType)
enum class ECharacterClass : uint8
{
	Elementalist,
	Warrior,
	Ranger
};

USTRUCT(BlueprintType)
struct FCharacterClassDefaultsInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Class Defaults") TSubclassOf<UGameplayEffect> PrimaryAttributes;
};

UCLASS()
class AURA_API UCharacterClassInfo : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults") TSubclassOf<UGameplayEffect> SecondaryAttributes;
	UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults") TSubclassOf<UGameplayEffect> VitalAttributes;
	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults") TMap<ECharacterClass, FCharacterClassDefaultsInfo> CharacterClassInformation;
	UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults", meta = (AllowPrivateAccess = "true")) TArray<TSubclassOf<UGameplayAbility>> CommonAbilities;
	UPROPERTY(EditDefaultsOnly, Category = "Damage", meta = (AllowPrivateAccess = "true")) TObjectPtr<UCurveTable> DamageCalculationCoeffs;

	FCharacterClassDefaultsInfo GetClassDefaultInfo(ECharacterClass CharacterClass);
};
