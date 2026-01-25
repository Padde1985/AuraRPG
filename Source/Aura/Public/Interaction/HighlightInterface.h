#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HighlightInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UHighlightInterface : public UInterface
{
	GENERATED_BODY()
};

// interface for all highlighting functionality, is implemented in character class and all highlightable actors (save spots, etc.)
class AURA_API IHighlightInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent) void HighlightActor();
	UFUNCTION(BlueprintNativeEvent) void UnHighlightActor();
	UFUNCTION(BlueprintNativeEvent) void SetMoveToLocation(FVector& OutDestination);
};
