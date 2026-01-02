#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "Interaction/SaveInterface.h"
#include "Interaction/HighLightInterface.h"
#include "../Aura.h"
#include "Checkpoint.generated.h"

class USphereComponent;

UCLASS()
class AURA_API ACheckpoint : public APlayerStart, public ISaveInterface, public IHighlightInterface
{
	GENERATED_BODY()
	
public:
	UPROPERTY(SaveGame, BlueprintReadWrite) bool bReached = false; //SaveGame makes it serializable
	UPROPERTY(EditAnywhere, BlueprintReadWrite )bool bBindOverlapCallback = true;

	ACheckpoint(const FObjectInitializer& ObjectInitializer);

	virtual bool ShouldLoadTransform_Implementation() override;
	virtual void LoadActor_Implementation() override;
	virtual void SetMoveToLocation_Implementation(FVector& OutDestination);
	virtual void HighlightActor_Implementation() override;
	virtual void UnHighlightActor_Implementation() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "", meta = (AllowPrivateAccess = "true")) int32 CustomDepthStencilOverride = CUSTOM_DEPTH_TAN;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true")) TObjectPtr<UStaticMeshComponent> CheckpointMesh;
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true")) TObjectPtr<USphereComponent> Sphere;
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true")) TObjectPtr<USceneComponent> MoveToComponent;

	UFUNCTION() virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(BlueprintImplementableEvent) void CheckpointReached(UMaterialInstanceDynamic* DynamicMaterialInstance);
	UFUNCTION(BlueprintCallable) void HandleGlowEffects();
	
	virtual void BeginPlay() override;
};
