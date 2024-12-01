#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraEnemy.generated.h"

class UWidgetComponent;
class UBehaviorTree;
class AAuraAIController;

UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable) FOnAttributeChangedSignature OnHealthChanged;
	UPROPERTY(BlueprintAssignable) FOnAttributeChangedSignature OnMaxHealthChanged;
	UPROPERTY(BlueprintReadWrite, Category = "Combat") TObjectPtr<AActor> CombatTarget;

	AAuraEnemy();

	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	virtual int32 GetLevel_Implementation() override;
	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	virtual void Die(const FVector& DeathImpulse) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override;
	virtual AActor* GetCombatTarget_Implementation() const override;
	virtual void Knockback(const FVector& Force) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<UWidgetComponent> HealthBar;

	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	virtual void InitializeDefaultAttributes() const override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Class Defaults", meta = (AllowPrivateAccess = "true")) int32 EnemyLevel = 1;
	UPROPERTY(BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true")) bool bHitReacting;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true")) float BaseWalkSpeed = 250.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true")) float LifeSpan = 5.f;
	UPROPERTY(EditAnywhere, Category = "AI", meta = (AllowPrivateAccess = "true")) TObjectPtr<UBehaviorTree> BehaviorTree;
	UPROPERTY() TObjectPtr<AAuraAIController> AIController;
};
