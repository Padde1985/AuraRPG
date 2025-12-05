#include "Actor/PointCollection.h"
#include "AbilitySystem/AuraAbilitysystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
APointCollection::APointCollection()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	this->Pt_0 = CreateDefaultSubobject<USceneComponent>("Pt_0");
	this->ImmutablePts.Add(this->Pt_0);
	SetRootComponent(this->Pt_0);

	CREATE_AND_SETUP_PT(this->Pt_1);
	CREATE_AND_SETUP_PT(this->Pt_2);
	CREATE_AND_SETUP_PT(this->Pt_3);
	CREATE_AND_SETUP_PT(this->Pt_4);
	CREATE_AND_SETUP_PT(this->Pt_5);
	CREATE_AND_SETUP_PT(this->Pt_6);
	CREATE_AND_SETUP_PT(this->Pt_7);
	CREATE_AND_SETUP_PT(this->Pt_8);
	CREATE_AND_SETUP_PT(this->Pt_9);
	CREATE_AND_SETUP_PT(this->Pt_10);
}

TArray<USceneComponent*> APointCollection::GetGroundPoints(const FVector& GroundLocation, const int32 NumPoints, float YawOverride)
{
	checkf(this->ImmutablePts.Num() >= NumPoints, TEXT("Attempted to access ImmutablePts out of bounds"));

	TArray<USceneComponent*> ArrayCopy;
	for (USceneComponent* Pt : this->ImmutablePts)
	{
		if (ArrayCopy.Num() >= NumPoints) return ArrayCopy;

		if (Pt != this->Pt_0)
		{
			FVector ToPoint = Pt->GetComponentLocation() - this->Pt_0->GetComponentLocation();
			ToPoint = ToPoint.RotateAngleAxis(YawOverride, FVector::UpVector);
			Pt->SetWorldLocation(this->Pt_0->GetComponentLocation() + ToPoint);
		}

		const FVector RaisedLocation = FVector(Pt->GetComponentLocation().X, Pt->GetComponentLocation().Y, Pt->GetComponentLocation().Z + 500.f);
		const FVector LoweredLocation = FVector(Pt->GetComponentLocation().X, Pt->GetComponentLocation().Y, Pt->GetComponentLocation().Z - 500.f);

		FHitResult HitResult;
		TArray<AActor*> IgnoreActors;
		UAuraAbilitysystemLibrary::GetLivePlayersWithinRadius(this, IgnoreActors, TArray<AActor*>(), 1500.f, GetActorLocation());

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActors(IgnoreActors);
		GetWorld()->LineTraceSingleByProfile(HitResult, RaisedLocation, LoweredLocation, FName("BlockAll"), QueryParams);

		const FVector AdjustedLocation = FVector(Pt->GetComponentLocation().X, Pt->GetComponentLocation().Y, HitResult.ImpactPoint.Z);
		Pt->SetWorldLocation(AdjustedLocation);
		Pt->SetWorldRotation(UKismetMathLibrary::MakeRotFromZ(HitResult.ImpactNormal));

		ArrayCopy.Add(Pt);
	}

	return ArrayCopy;
}

// Called when the game starts or when spawned
void APointCollection::BeginPlay()
{
	Super::BeginPlay();
	
}