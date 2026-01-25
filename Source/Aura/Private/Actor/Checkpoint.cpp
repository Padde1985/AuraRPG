#include "Actor/Checkpoint.h"
#include "Components/SphereComponent.h"
#include "Interaction/PlayerInterface.h"
#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"

// create the checkpoint actor in the world
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

	this->MoveToComponent = CreateDefaultSubobject<USceneComponent>("Move To Component");
	this->MoveToComponent->SetupAttachment(GetRootComponent());
}

// getter for transform
bool ACheckpoint::ShouldLoadTransform_Implementation()
{
	return false;
}

// handle glow effects after actor is fully loaded from save game
void ACheckpoint::LoadActor_Implementation()
{
	if (this->bReached)	this->HandleGlowEffects();
}

// get move to location if auto running to checkpoint
void ACheckpoint::SetMoveToLocation_Implementation(FVector& OutDestination)
{
	OutDestination = this->MoveToComponent->GetComponentLocation();
}

// highlight checkpoint when reaching it
void ACheckpoint::HighlightActor_Implementation()
{
	if(!this->bReached) this->CheckpointMesh->SetRenderCustomDepth(true);
}

// unhighlight actor (if not reached yet)
void ACheckpoint::UnHighlightActor_Implementation()
{
	this->CheckpointMesh->SetRenderCustomDepth(false);
}

// handle glow effects depending on reached status
void ACheckpoint::HandleGlowEffects()
{
	this->Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UMaterialInstanceDynamic* MI = UMaterialInstanceDynamic::Create(this->CheckpointMesh->GetMaterial(0), this);
	this->CheckpointMesh->SetMaterial(0, MI);
	this->CheckpointReached(MI);
}

// when overlapping the checkpoint (reaching the checkpoint)
void ACheckpoint::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UPlayerInterface>())
	{
		this->bReached = true;
		if (AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			const UWorld* World = GetWorld();
			FString MapName = World->GetMapName();
			MapName.RemoveFromStart(World->StreamingLevelsPrefix);

			GameMode->SaveWorldState(GetWorld(), MapName);
		}

		IPlayerInterface::Execute_SaveProgress(OtherActor, PlayerStartTag);
		this->HandleGlowEffects();
	}
}

// bind callback for stepping on the checkpoint move to location
void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();

	if(this->bBindOverlapCallback) this->Sphere->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::OnSphereOverlap);

	this->CheckpointMesh->SetCustomDepthStencilValue(this->CustomDepthStencilOverride);
	this->CheckpointMesh->SetRenderCustomDepth(false); //will be activated while hovering
}