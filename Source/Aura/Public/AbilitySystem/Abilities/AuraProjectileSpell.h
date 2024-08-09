#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraProjectileSpell.generated.h"

class AAuraProjectile;
class UGameplayEffect;

UCLASS()
class AURA_API UAuraProjectileSpell : public UAuraGameplayAbility
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Classes", meta = (AllowPrivateAccess = "true")) TSubclassOf<AAuraProjectile> ProjectileClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Classes", meta = (AllowPrivateAccess = "true")) TSubclassOf<UGameplayEffect> DamageEffectClass;

	UFUNCTION(BlueprintCallable, Category = "Projectile") void SpawnProjectile(const FVector& ProjectileTargetLocation);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
