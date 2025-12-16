#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AuraAbilityTypes.h"
#include "AuraProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UNiagaraSystem;
struct FDamageEffectParams;

UCLASS()
class AURA_API AAuraProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(VisibleAnywhere, Category = "Components") TObjectPtr<UProjectileMovementComponent> ProjectileMovement;
	UPROPERTY(BlueprintReadWrite, Category = "", meta = (ExposeOnSpawn = "true")) FDamageEffectParams DamageEffectParams;
	UPROPERTY() TObjectPtr<USceneComponent> HomingTargetSceneComponent;

	// Sets default values for this actor's properties
	AAuraProjectile();

protected:
	UPROPERTY(VisibleAnywhere) TObjectPtr<UAudioComponent> FlySoundComponent;

	bool bHit = false;

	UFUNCTION() virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(BlueprintCallable) virtual void OnHit();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	bool IsValidOverlap(AActor* OtherActor);

private:
	UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true")) TObjectPtr<USphereComponent> Sphere;
	UPROPERTY(EditAnywhere, Category = "Effects", meta = (AllowPrivateAccess = "true")) TObjectPtr<UNiagaraSystem> ImpactEffect;
	UPROPERTY(EditAnywhere, Category = "Effects", meta = (AllowPrivateAccess = "true")) TObjectPtr<USoundBase> ImpactSound;
	UPROPERTY(EditAnywhere, Category = "Effects", meta = (AllowPrivateAccess = "true")) TObjectPtr<USoundBase> FlySound;
	UPROPERTY(EditDefaultsOnly, Category = "", meta = (AllowPrivateAccess = "true")) float LifeSpan = 15.f;
};
