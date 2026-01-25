#include "Actor/AuraFireBall.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitysystemLibrary.h"
#include "Components/AudioComponent.h"
#include "GameplayCueManager.h"
#include "AuraGameplayTags.h"

// the fireball does not tick and has to be activated by the ability
AAuraFireBall::AAuraFireBall()
{
	ProjectileMovement->PrimaryComponentTick.bCanEverTick = false;
	ProjectileMovement->SetAutoActivate(false);
}

// outgoing timeline will be activated for projectile movement
void AAuraFireBall::BeginPlay()
{
	Super::BeginPlay();

	this->StartOutgoingTimeline();
}

// callback for overlapping with target
void AAuraFireBall::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValidOverlap(OtherActor)) return;

	// if server -> destroy the object when the server player launched the spell
	if (HasAuthority())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			const FVector DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
			DamageEffectParams.DeathImpulse = DeathImpulse;
			
			DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
			UAuraAbilitysystemLibrary::ApplyDamageEffect(this->DamageEffectParams);
		}
	}
}

// if fireball hit target start gameplay cue
void AAuraFireBall::OnHit()
{
	// execute local GameplayCue
	if (GetOwner())
	{
		FGameplayCueParameters CueParams;
		CueParams.Location = GetActorLocation();
		UGameplayCueManager::ExecuteGameplayCue_NonReplicated(GetOwner(), FAuraGameplayTags::Get().GameplayCue_FireBlast, CueParams);
	}

	if (FlySoundComponent)
	{
		FlySoundComponent->Stop();
		FlySoundComponent->DestroyComponent();
	}

	bHit = true;
}
