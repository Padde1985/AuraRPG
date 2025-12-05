#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PointCollection.generated.h"

#define CREATE_AND_SETUP_PT(Pt_X) \
    Pt_X = CreateDefaultSubobject<USceneComponent>(TEXT(#Pt_X)); \
    ImmutablePts.Add(Pt_X); \
    Pt_X->SetupAttachment(GetRootComponent());

UCLASS()
class AURA_API APointCollection : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APointCollection();

	UFUNCTION(BlueprintPure) TArray<USceneComponent*> GetGroundPoints(const FVector& GroundLocation, const int32 NumPoints, float YawOverride = 0);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true")) TObjectPtr<USceneComponent> Pt_0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true")) TObjectPtr<USceneComponent> Pt_1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true")) TObjectPtr<USceneComponent> Pt_2;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true")) TObjectPtr<USceneComponent> Pt_3;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true")) TObjectPtr<USceneComponent> Pt_4;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true")) TObjectPtr<USceneComponent> Pt_5;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true")) TObjectPtr<USceneComponent> Pt_6;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true")) TObjectPtr<USceneComponent> Pt_7;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true")) TObjectPtr<USceneComponent> Pt_8;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true")) TObjectPtr<USceneComponent> Pt_9;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true")) TObjectPtr<USceneComponent> Pt_10;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "", meta = (AllowPrivateAccess = "true") )TArray<USceneComponent*> ImmutablePts;
};
