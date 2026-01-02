#pragma once

#include "CoreMinimal.h"
#include "Actor/Checkpoint.h"
#include "MapEntrance.generated.h"

UCLASS()
class AURA_API AMapEntrance : public ACheckpoint
{
	GENERATED_BODY()
	
public:
	AMapEntrance(const FObjectInitializer& ObjectInitializer);

	virtual void HighlightActor_Implementation() override;
	virtual void LoadActor_Implementation() override;

protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true")) TSoftObjectPtr<UWorld> DestinationMap;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true")) FName DestinationPlayerStartTag;
};
