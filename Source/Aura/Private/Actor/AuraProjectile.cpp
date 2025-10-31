#include "Actor/AuraProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "../Aura.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitysystemLibrary.h"
#include "AuraAbilityTypes.h"

// Sets default values
AAuraProjectile::AAuraProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true; // spawn projectile on server so that server can controll everything

	this->Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	this->Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	this->Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	this->Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	this->Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	this->Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	this->Sphere->SetCollisionObjectType(ECC_Projectile);
	SetRootComponent(this->Sphere);

	this->ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("Projectile Movement");
	this->ProjectileMovement->InitialSpeed = 550.f;
	this->ProjectileMovement->MaxSpeed = 550.f;
	this->ProjectileMovement->ProjectileGravityScale = 0.f; // disable gravity
}

void AAuraProjectile::Destroyed()
{
	// in case of client -> handle sound and impact effect before actually destroying the object and avoid calling the overlap event
	if (!this->bHit && !HasAuthority())	this->OnHit();

	if (this->FlySoundComponent)
	{
		this->FlySoundComponent->Stop();
		this->FlySoundComponent->DestroyComponent();
	}

	Super::Destroyed();
}

void AAuraProjectile::OnHit()
{
	UGameplayStatics::PlaySoundAtLocation(this, this->ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
	if (this->FlySoundComponent)
	{
		this->FlySoundComponent->Stop();
		this->FlySoundComponent->DestroyComponent();
	}
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, this->ImpactEffect, GetActorLocation());
	this->bHit = true;
}

// Called when the game starts or when spawned
void AAuraProjectile::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(this->LifeSpan);
	SetReplicateMovement(true);

	this->Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnSphereOverlap);
	this->FlySoundComponent = UGameplayStatics::SpawnSoundAttached(this->FlySound, GetRootComponent());
}

void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(this->DamageEffectParams.SourceAbilitySystemComponent)) return;

	AActor* SourceAvatarActor = this->DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();
	if (SourceAvatarActor == OtherActor) return;
	if (!UAuraAbilitysystemLibrary::IsNotFriend(SourceAvatarActor, OtherActor)) return;

	if (!this->bHit) this->OnHit();

	// if server -> destroy the object when the server player launched the spell
	if (HasAuthority())
	{
		if (UAbilitySystemComponent * TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			const FVector DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
			DamageEffectParams.DeathImpulse = DeathImpulse;
			const bool bKnockback = FMath::RandRange(1, 100) < DamageEffectParams.KnockbackChance;
			if (bKnockback)
			{
				FRotator Rotation = GetActorRotation();
				Rotation.Pitch = 45.f;
				const FVector KnockbackDirection = Rotation.Vector();
				const FVector KnockbackForce = GetActorForwardVector() * DamageEffectParams.KnockbackForceMagnitude;
				DamageEffectParams.KnockbackForce = KnockbackForce;
			}
			DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
			UAuraAbilitysystemLibrary::ApplyDamageEffect(this->DamageEffectParams);
		}

		Destroy();
	}
	// if client -> just set a parameter that the overlap event was fired
	else this->bHit = true;
}

