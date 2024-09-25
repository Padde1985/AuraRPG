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
	if (!this->bHit && !HasAuthority())
	{
		UGameplayStatics::PlaySoundAtLocation(this, this->ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
		if(this->FlySoundComponent) this->FlySoundComponent->Stop();
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, this->ImpactEffect, GetActorLocation());
		this->bHit = true;
	}
	Super::Destroyed();
}

// Called when the game starts or when spawned
void AAuraProjectile::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(this->LifeSpan);

	this->Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnSphereOverlap);
	this->FlySoundComponent = UGameplayStatics::SpawnSoundAttached(this->FlySound, GetRootComponent());
}

void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!DamageEffectSpecHandle.Data.IsValid() || DamageEffectSpecHandle.Data.Get()->GetContext().GetEffectCauser() == OtherActor) return;
	if (!UAuraAbilitysystemLibrary::IsNotFriend(DamageEffectSpecHandle.Data.Get()->GetContext().GetEffectCauser(), OtherActor)) return;

	if (!this->bHit)
	{
		UGameplayStatics::PlaySoundAtLocation(this, this->ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
		if (this->FlySoundComponent) this->FlySoundComponent->Stop();
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, this->ImpactEffect, GetActorLocation());
		this->bHit = true;
	}

	// if server -> destroy the object when the server player launched the spell
	if (HasAuthority())
	{
		if (UAbilitySystemComponent * TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*this->DamageEffectSpecHandle.Data.Get());
		}

		Destroy();
	}
	// if client -> just set a parameter that the overlap event was fired
	else
	{
		this->bHit = true;
	}
}

