#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraProjectileSpell.h"
#include "AuraFireBolt.generated.h"

UCLASS()
class AURA_API UAuraFireBolt : public UAuraProjectileSpell
{
	GENERATED_BODY()
	
public:
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;

	UFUNCTION(BlueprintCallable, Category = "Projectile") void SpawnProjectiles(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, AActor* HomingTarget, bool bPitchOverride = false, float Pitch = 0.f);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Firebolt", meta = (AllowPrivateAccess = "true")) float ProjectileSpread = 90.f;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "", meta = (AllowPrivateAccess = "true")) int32 MaxProjectiles = 5;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "", meta = (AllowPrivateAccess = "true")) float HomingAccelerationMin = 1600.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "", meta = (AllowPrivateAccess = "true")) float HomingAccelerationMax = 3200.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "", meta = (AllowPrivateAccess = "true")) bool bLaunchHomingProjectiles = true;
};
