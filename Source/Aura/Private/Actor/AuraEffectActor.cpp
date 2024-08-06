#include "Actor/AuraEffectActor.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

// Sets default values
AAuraEffectActor::AAuraEffectActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}

// Called when the game starts or when spawned
void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
}

// apply effect to a specific target
void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GamePlayEffectClass)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (TargetASC == nullptr) return;

	//apply gameplay effect
	check(GamePlayEffectClass);
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);

	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GamePlayEffectClass, this->ActorLevel, EffectContextHandle);
	TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data);
}

// trigger effect when overlapping (standing in the flames or walking over a potion)
void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	if (this->InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap) this->ApplyEffectToTarget(TargetActor, this->InstantGameplayEffectClass);
	if (this->DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap) this->ApplyEffectToTarget(TargetActor, this->DurationGameplayEffectClass);
	if (this->InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap) this->ApplyEffectToTarget(TargetActor, this->InfiniteGameplayEffectClass);
}

// trigger effect when leaving the overlapped actor
void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
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

