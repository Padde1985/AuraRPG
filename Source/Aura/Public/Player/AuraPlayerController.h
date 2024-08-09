#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "AuraPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class IEnemyInterface;
class UAuraInputConfig;
class UAuraAbilitySystemComponent;
class USplineComponent;

struct FInputActionValue;

UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AAuraPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void PlayerTick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true")) TObjectPtr<UInputMappingContext> AuraContext;
	UPROPERTY(VisibleAnywhere) TObjectPtr<USplineComponent> Spline;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true")) UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true")) UInputAction* ShiftAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input") TObjectPtr<UAuraInputConfig> InputConfig;
	UPROPERTY() TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Input", meta = (AllowPrivateAccess = "true")) float AutoRunAcceptanceRadius = 50.f;
	TScriptInterface<IEnemyInterface> LastActor; // TObjectPrt is only used for Objects, not interfaces
	TScriptInterface<IEnemyInterface> ThisActor;

	FVector CachedDestination = FVector::ZeroVector;
	float FollowTime = 0.f;
	float ShortPressThreshold = 0.5f;
	bool bAutoRunning = false;
	bool bTargeting = false;
	bool bShiftDown = false;
	FHitResult CursorHit;

	void Move(const FInputActionValue& InputActionValue);
	void ShiftPressed();
	void ShiftReleased();
	void CursorTrace();
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld (FGameplayTag InputTag);
	UAuraAbilitySystemComponent* GetASC();
	void AutoRun();
};
