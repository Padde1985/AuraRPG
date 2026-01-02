#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AuraEffectActor.generated.h"

class UGameplayEffect;

UENUM(BlueprintType)
enum class EEffectApplicationPolicy : uint8
{
	ApplyOnOverlap UMETA(DisplayName = "Apply on Everlap"),
	ApplyOnEndOverlap UMETA(DisplayName = "Apply on end Overlap"),
	DoNotApply UMETA(DisplayName = "Do not apply")
};

UENUM(BlueprintType)
enum class EEffectRemovalPolicy : uint8
{
	RemoveOnEndOverlap UMETA(DisplayName = "Remove on End Overlap"),
	DoNotRemove UMETA(DisplayName = "Do not remove")
};

UENUM(BlueprintType)
enum class EActorDestroyPolicy : uint8
{
	DestroyOnEndOverlap UMETA(DisplayName = "Destroy on End Overlap"),
	DoNotDestroy UMETA(DisplayName = "Do not destroy")
};

UCLASS()
class AURA_API AAuraEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAuraEffectActor();

	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effect", meta = (AllowPrivateAccess = "true")) TSubclassOf<UGameplayEffect> InstantGameplayEffectClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effect", meta = (AllowPrivateAccess = "true")) TSubclassOf<UGameplayEffect> DurationGameplayEffectClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effect", meta = (AllowPrivateAccess = "true")) TSubclassOf<UGameplayEffect> InfiniteGameplayEffectClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effect", meta = (AllowPrivateAccess = "true")) EEffectApplicationPolicy InstantEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effect", meta = (AllowPrivateAccess = "true")) EEffectApplicationPolicy DurationEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effect", meta = (AllowPrivateAccess = "true")) EEffectApplicationPolicy InfiniteEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effect", meta = (AllowPrivateAccess = "true")) EEffectRemovalPolicy InfiniteEffectRemovalPolicy = EEffectRemovalPolicy::RemoveOnEndOverlap;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effect", meta = (AllowPrivateAccess = "true")) EActorDestroyPolicy DestroyPolicy = EActorDestroyPolicy::DestroyOnEndOverlap;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effect", meta = (AllowPrivateAccess = "true")) bool bApplyEffectsToEnemies = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Applied Effect", meta = (AllowPrivateAccess = "true")) float ActorLevel = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Movement", meta = (AllowPrivateAccess = "true")) bool bRotates = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Movement", meta = (AllowPrivateAccess = "true")) float RotationRate = 45.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Movement", meta = (AllowPrivateAccess = "true")) bool bSinusoidalMovement = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Movement", meta = (AllowPrivateAccess = "true")) float SineAmplitude = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Movement", meta = (AllowPrivateAccess = "true")) float SinePeriodConstant = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Movement", meta = (AllowPrivateAccess = "true")) FVector InitialLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Movement", meta = (AllowPrivateAccess = "true")) FVector CalculatedLocation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup Movement", meta = (AllowPrivateAccess = "true")) FRotator CalculatedRotation;

	UFUNCTION(BlueprintCallable) void ApplyEffectToTarget(AActor* TargetActor, TSubclassOf< UGameplayEffect> GamePlayEffectClass);
	UFUNCTION(BlueprintCallable) void OnOverlap(AActor* TargetActor);
	UFUNCTION(BlueprintCallable) void OnEndOverlap(AActor* TargetActor);
	UFUNCTION(BlueprintCallable) void StartSinusoidalMovement();
	UFUNCTION(BlueprintCallable) void StartRotation();

private:
	float RunningTime = 0.f;

	void ItemMovememnt(float DeltaTime);
};