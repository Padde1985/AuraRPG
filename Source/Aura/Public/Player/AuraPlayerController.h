#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class IEnemyInterface;

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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true")) UInputAction* MoveAction;
	TScriptInterface<IEnemyInterface> LastActor; // TObjectPrt is only used for Objects, not interfaces
	TScriptInterface<IEnemyInterface> ThisActor;

	void Move(const FInputActionValue& InputActionValue);
	void CursorTrace();
};
