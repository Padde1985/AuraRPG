#include "AbilitySystem/Abilities/AuraSummonAbility.h"

TArray<FVector> UAuraSummonAbility::GetSpawnLocations()
{
	TArray<FVector> SpawnLocations;

	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	const float DeltaSpread = this->SpawnSpread / this->NumMinions;

	// rotate Vector around the UpVector (yaw axis) => rotating left and right
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-this->SpawnSpread / 2.f, FVector::UpVector);
	// spawn minions along evenly distributed lines within the spread
	for (int32 i = 0; i < this->NumMinions; i++)
	{
		const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
		FVector ChosenSpawnLocation = Location + Direction * FMath::FRandRange(this->MinSpawnDistance, this->MaxSpawnDistance);

		// make sure that Spawning point is always on the floor
		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(Hit, ChosenSpawnLocation + FVector(0.f, 0.f, 400.f), ChosenSpawnLocation - FVector(0.f, 0.f, 400.f), ECollisionChannel::ECC_Visibility);
		if (Hit.bBlockingHit) ChosenSpawnLocation = Hit.ImpactPoint;

		SpawnLocations.Add(ChosenSpawnLocation);
	}
	const FVector RightOfSpread = Forward.RotateAngleAxis(this->SpawnSpread / 2.f, FVector::UpVector);

	return SpawnLocations;
}

TSubclassOf<APawn> UAuraSummonAbility::GetRandomMinionClass()
{
	const int32 Selection = FMath::RandRange(0, this->MinionClasses.Num() - 1);
	return this->MinionClasses[Selection];
}
