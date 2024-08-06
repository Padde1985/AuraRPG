#include "Actor/AuraProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

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
	SetRootComponent(this->Sphere);

	this->ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("Projectile Movement");
	this->ProjectileMovement->InitialSpeed = 550.f;
	this->ProjectileMovement->MaxSpeed = 550.f;
	this->ProjectileMovement->ProjectileGravityScale = 0.f; // disable gravity
}

// Called when the game starts or when spawned
void AAuraProjectile::BeginPlay()
{
	Super::BeginPlay();

	this->Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnSphereOverlap);
}

void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

