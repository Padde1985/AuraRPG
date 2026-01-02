#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "Interaction/HighlightInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "Interaction/SaveInterface.h"
#include "AuraEnemy.generated.h"

class UWidgetComponent;
class UBehaviorTree;
class AAuraAIController;

UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface, public ISaveInterface, public IHighlightInterface
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable) FOnAttributeChangedSignature OnHealthChanged;
	UPROPERTY(BlueprintAssignable) FOnAttributeChangedSignature OnMaxHealthChanged;
	UPROPERTY(BlueprintReadWrite, Category = "Combat") TObjectPtr<AActor> CombatTarget;

	AAuraEnemy();

	virtual void HighlightActor_Implementation() override;
	virtual void UnHighlightActor_Implementation() override;
	virtual int32 GetLevel_Implementation() override;
	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	virtual void Die(const FVector& DeathImpulse) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override;
	virtual AActor* GetCombatTarget_Implementation() const override;
	virtual void Knockback(const FVector& Force) override;
	virtual void SetMoveToLocation_Implementation(FVector& OutDestination) override;
	void SetEnemyLevel(int32 Level);
	void SetCharacterClass(ECharacterClass Class);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<UWidgetComponent> HealthBar;

	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	virtual void InitializeDefaultAttributes() const override;
	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount) override;

	UFUNCTION(BlueprintImplementableEvent) void SpawnLoot();

private:
	UPROPERTY(BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true")) bool bHitReacting;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true")) float LifeSpan = 5.f;
	UPROPERTY(EditAnywhere, Category = "AI", meta = (AllowPrivateAccess = "true")) TObjectPtr<UBehaviorTree> BehaviorTree;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Class Defaults", meta = (AllowPrivateAccess = "true")) int32 EnemyLevel = 1;
	UPROPERTY() TObjectPtr<AAuraAIController> AIController;
};
