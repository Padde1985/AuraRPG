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
class UDamageTextComponent;
class UNiagaraSystem;
class AMagicCircle;

struct FInputActionValue;

UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AAuraPlayerController();

	UFUNCTION(Client, Reliable) void ShowDamageNumber(float DamageAmount, ACharacter* TargetCharacter, bool bIsBlocked, bool bIsCritical);
	UFUNCTION(BlueprintCallable) void ShowMagicCircle(UMaterialInterface* DecalMaterial);
	UFUNCTION(BlueprintCallable) void HideMagicCircle();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void PlayerTick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true")) TObjectPtr<UInputMappingContext> AuraContext;
	UPROPERTY(VisibleAnywhere) TObjectPtr<USplineComponent> Spline;
	UPROPERTY(EditDefaultsOnly) TObjectPtr<UNiagaraSystem> ClickNiagaraSystem;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true")) UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true")) UInputAction* ShiftAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input") TObjectPtr<UAuraInputConfig> InputConfig;
	UPROPERTY() TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Input", meta = (AllowPrivateAccess = "true")) float AutoRunAcceptanceRadius = 50.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "", meta = (AllowPrivateAccess = "true"))TSubclassOf<UDamageTextComponent> DamageTextComponentClass;
	UPROPERTY(EditDefaultsOnly) TSubclassOf<AMagicCircle> MagicCircleClass;

	TScriptInterface<IEnemyInterface> LastActor; // TObjectPrt is only used for Objects, not interfaces
	TScriptInterface<IEnemyInterface> ThisActor;
	FVector CachedDestination = FVector::ZeroVector;
	float FollowTime = 0.f;
	float ShortPressThreshold = 0.5f;
	bool bAutoRunning = false;
	bool bTargeting = false;
	bool bShiftDown = false;
	FHitResult CursorHit;
	TObjectPtr<AMagicCircle> MagicCircle;

	void Move(const FInputActionValue& InputActionValue);
	void ShiftPressed();
	void ShiftReleased();
	void CursorTrace();
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld (FGameplayTag InputTag);
	UAuraAbilitySystemComponent* GetASC();
	void AutoRun();
	void UpdateMagicCircleLocation();
};
