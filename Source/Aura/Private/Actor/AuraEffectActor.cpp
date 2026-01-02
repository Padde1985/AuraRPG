#include "Actor/AuraEffectActor.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AAuraEffectActor::AAuraEffectActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}

void AAuraEffectActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	this->RunningTime += DeltaTime;
	const float SinePeriod = 2 * PI / this->SinePeriodConstant;
	if (this->RunningTime > SinePeriod) this->RunningTime = 0.f;

	this->ItemMovememnt(DeltaTime);
}

// Called when the game starts or when spawned
void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();

	this->InitialLocation = GetActorLocation();
	this->CalculatedLocation = this->InitialLocation;
	this->CalculatedRotation = GetActorRotation();
}

// apply effect to a specific target
void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GamePlayEffectClass)
{
	if (TargetActor->ActorHasTag(FName("Enemy")) && !this->bApplyEffectsToEnemies) return;

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (TargetASC == nullptr) return;

	//apply gameplay effect
	check(GamePlayEffectClass);
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);

	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GamePlayEffectClass, this->ActorLevel, EffectContextHandle);
	TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data);

	if (this->DestroyPolicy == EActorDestroyPolicy::DestroyOnEndOverlap && EffectSpecHandle.Data->Def->DurationPolicy != EGameplayEffectDurationType::Infinite) Destroy();
}

// trigger effect when overlapping (standing in the flames or walking over a potion)
void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	if (TargetActor->ActorHasTag(FName("Enemy")) && !this->bApplyEffectsToEnemies) return;

	if (this->InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap) this->ApplyEffectToTarget(TargetActor, this->InstantGameplayEffectClass);
	if (this->DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap) this->ApplyEffectToTarget(TargetActor, this->DurationGameplayEffectClass);
	if (this->InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap) this->ApplyEffectToTarget(TargetActor, this->InfiniteGameplayEffectClass);
}

// trigger effect when leaving the overlapped actor
void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	if (TargetActor->ActorHasTag(FName("Enemy")) && !this->bApplyEffectsToEnemies) return;

	if (this->InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap) this->ApplyEffectToTarget(TargetActor, this->InstantGameplayEffectClass);
	if (this->DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap) this->ApplyEffectToTarget(TargetActor, this->DurationGameplayEffectClass);
	if (this->InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap) this->ApplyEffectToTarget(TargetActor, this->InfiniteGameplayEffectClass);
	// only infinite effects have to be removed as instants happen just once and immediately and durations end after the configured amount of seconds
	if (this->InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (TargetASC == nullptr) return;
		TargetASC->RemoveActiveGameplayEffectBySourceEffect(this->InfiniteGameplayEffectClass, TargetASC, 1);
	}
}

void AAuraEffectActor::StartSinusoidalMovement()
{
	this->bSinusoidalMovement = true;
	this->InitialLocation = GetActorLocation();
	this->CalculatedLocation = this->InitialLocation;
}

void AAuraEffectActor::StartRotation()
{
	this->bRotates = true;
	this->CalculatedRotation = GetActorRotation();
}

void AAuraEffectActor::ItemMovememnt(float DeltaTime)
{
	if (this->bRotates)
	{
		const FRotator DeltaRtotation(0.f, DeltaTime * this->RotationRate, 0.f);
		this->CalculatedRotation = UKismetMathLibrary::ComposeRotators(this->CalculatedRotation, DeltaRtotation);
	}

	if (this->bSinusoidalMovement)
	{
		const float Sine = FMath::Sin(this->RunningTime * this->SinePeriodConstant) * this->SineAmplitude;
		this->CalculatedLocation = this->InitialLocation + FVector(0.f, 0.f, Sine);
	}
}

