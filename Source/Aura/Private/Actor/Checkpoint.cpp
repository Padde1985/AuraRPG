#include "Actor/Checkpoint.h"
#include "Components/SphereComponent.h"
#include "Interaction/PlayerInterface.h"

ACheckpoint::ACheckpoint(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	this->CheckpointMesh = CreateDefaultSubobject<UStaticMeshComponent>("Checkpoint Mesh");
	this->CheckpointMesh->SetupAttachment(GetRootComponent());
	this->CheckpointMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	this->CheckpointMesh->SetCollisionResponseToAllChannels(ECR_Block);

	this->Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	this->Sphere->SetupAttachment(this->CheckpointMesh);
	this->Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	this->Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	this->Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ACheckpoint::HandleGlowEffects()
{
	this->Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UMaterialInstanceDynamic* MI = UMaterialInstanceDynamic::Create(this->CheckpointMesh->GetMaterial(0), this);
	this->CheckpointMesh->SetMaterial(0, MI);
	this->CheckpointReached(MI);
}

void ACheckpoint::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UPlayerInterface>())
	{
		IPlayerInterface::Execute_SaveProgress(OtherActor, PlayerStartTag);
		this->HandleGlowEffects();
	}
}

void ACheckpoint::BeginPlay()
{
	this->Sphere->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::OnSphereOverlap);
}
