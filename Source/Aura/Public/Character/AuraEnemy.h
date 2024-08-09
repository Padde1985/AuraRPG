#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraEnemy.generated.h"

class UWidgetComponent;

UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable) FOnAttributeChangedSignature OnHealthChanged;
	UPROPERTY(BlueprintAssignable) FOnAttributeChangedSignature OnMaxHealthChanged;

	AAuraEnemy();

	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	virtual int32 GetLevel() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<UWidgetComponent> HealthBar;

	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Class Defaults", meta = (AllowPrivateAccess = "true")) int32 EnemyLevel = 1;
};
