#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

class UAnimMontage;

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

class AURA_API ICombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual int32 GetLevel(); // no pure function (=0), otherwise even the Base class had to implement this function
	virtual FVector GetCombatSocketLocation();
	virtual void Die() = 0;
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable) void UpdateWarpTarget(const FVector& FacingTarget);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) UAnimMontage* GetHitReactMontage(); // automatically creates the _implementation function
};
