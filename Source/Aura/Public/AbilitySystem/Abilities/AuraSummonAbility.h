#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraSummonAbility.generated.h"

UCLASS()
class AURA_API UAuraSummonAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable) TArray<FVector> GetSpawnLocations();
	UFUNCTION(BlueprintPure, Category = "Summoning") TSubclassOf<APawn> GetRandomMinionClass();

private:
	UPROPERTY(EditAnywhere, Category = "Summoning", meta = (AllowPrivateAccess = "true"))int32 NumMinions = 5;
	UPROPERTY(EditDefaultsOnly, Category = "Summoning", meta = (AllowPrivateAccess = "true")) TArray<TSubclassOf<APawn>> MinionClasses;
	UPROPERTY(EditDefaultsOnly, Category = "Summoning", meta = (AllowPrivateAccess = "true")) float MaxSpawnDistance = 250.f;
	UPROPERTY(EditDefaultsOnly, Category = "Summoning", meta = (AllowPrivateAccess = "true")) float MinSpawnDistance = 50.f;
	UPROPERTY(EditDefaultsOnly, Category = "Summoning", meta = (AllowPrivateAccess = "true")) float SpawnSpread = 90.f;
};
