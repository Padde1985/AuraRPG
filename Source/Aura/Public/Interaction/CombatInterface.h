#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "CombatInterface.generated.h"

class UAnimMontage;

USTRUCT(BlueprintType)
struct FTaggedMontage
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) UAnimMontage* Montage = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FGameplayTag MontageTag;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

class AURA_API ICombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual int32 GetLevel(); // no pure function (=0), otherwise even the Base class had to implement this function
	virtual void Die() = 0;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) FVector GetCombatSocketLocation(const FGameplayTag& MontageTag);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable) void UpdateWarpTarget(const FVector& FacingTarget);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) UAnimMontage* GetHitReactMontage(); // automatically creates the _implementation function
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) bool IsDead() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) AActor* GetAvatar();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) TArray<FTaggedMontage> GetAttackMontages();
};
