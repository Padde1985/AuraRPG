#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "Interaction/SaveInterface.h"
#include "Checkpoint.generated.h"

class USphereComponent;

UCLASS()
class AURA_API ACheckpoint : public APlayerStart, public ISaveInterface
{
	GENERATED_BODY()
	
public:
	UPROPERTY(SaveGame, BlueprintReadOnly) bool bReached = false; //SaveGame makes it serializable

	ACheckpoint(const FObjectInitializer& ObjectInitializer);

	virtual bool ShouldLoadTransform_Implementation() override;
	virtual void LoadActor_Implementation() override;

protected:
	UFUNCTION() virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(BlueprintImplementableEvent) void CheckpointReached(UMaterialInstanceDynamic* DynamicMaterialInstance);
	
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true")) TObjectPtr<UStaticMeshComponent> CheckpointMesh;
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true")) TObjectPtr<USphereComponent> Sphere;

	void HandleGlowEffects();
};
