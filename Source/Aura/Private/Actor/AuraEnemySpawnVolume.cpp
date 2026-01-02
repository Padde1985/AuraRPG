#include "Actor/AuraEnemySpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Interaction/PlayerInterface.h"
#include "Actor/AuraEnemySpawnPoint.h"

// Sets default values
AAuraEnemySpawnVolume::AAuraEnemySpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	this->Box = CreateDefaultSubobject<UBoxComponent>("Box");
	SetRootComponent(this->Box);
	this->Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	this->Box->SetCollisionObjectType(ECC_WorldStatic);
	this->Box->SetCollisionResponseToAllChannels(ECR_Ignore);
	this->Box->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Overlap);
}

void AAuraEnemySpawnVolume::LoadActor_Implementation()
{
	if (this->bReached) Destroy();
}

// Called when the game starts or when spawned
void AAuraEnemySpawnVolume::BeginPlay()
{
	Super::BeginPlay();

	this->Box->OnComponentBeginOverlap.AddDynamic(this, &AAuraEnemySpawnVolume::OnBoxOverlap);
}

void AAuraEnemySpawnVolume::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->Implements<UPlayerInterface>()) return;

	this->bReached = true;

	for (AAuraEnemySpawnPoint* Point : this->SpawnPoints)
	{
		if (IsValid(Point))	Point->SpawnEnemy();
	}
	this->Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
