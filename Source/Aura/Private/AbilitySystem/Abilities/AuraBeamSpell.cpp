#include "AbilitySystem/Abilities/AuraBeamSpell.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "AbilitySystem/AuraAbilitysystemLibrary.h"

FString UAuraBeamSpell::GetDescription(int32 Level)
{
	// get scaled damage from curve table for a given level
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);

	if (Level == 1)
	{
		return FString::Printf(TEXT("<Title>ELECTRICUTE</>\n\n"
							   "<Default>Casts one beam of Lightning dealing </><Damage>%d </>"
							   "<Default>lightning damage with a chance to stun</>\n\n"
							   "<Small>Level: </><Level>%d</>\n"
							   "<Small>ManaCost: </><ManaCost>%.0f</>\n"
							   "<Small>Cooldown: </><Cooldown>%.1f</>"),
							   ScaledDamage, Level, ManaCost, Cooldown);
	}
	else
	{
		return FString::Printf(TEXT("<Title>ELECTRICUTE</>\n\n"
							   "<Default>Casts one beam of Lightning impacting %d additional targets dealing </><Damage>%d </>"
							   "<Default>lightning damage with a chance to stun</>\n\n"
							   "<Small>Level: </><Level>%d</>\n"
							   "<Small>ManaCost: </><ManaCost>%.0f</>\n"
							   "<Small>Cooldown: </><Cooldown>%.1f</>"),
							   FMath::Min(Level - 1, this->MaxNumShockTargets), ScaledDamage, Level, ManaCost, Cooldown);
	}
}

FString UAuraBeamSpell::GetNextLevelDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);

	return FString::Printf(TEXT("<Title>Next Level</>\n\n"
						   "<Default>Casts one beam of Lightning impacting %d additional targets dealing </><Damage>%d </>"
						   "<Default>lightning damage with a chance to stun</>\n\n"
						   "<Small>Level: </><Level>%d</>\n"
						   "<Small>ManaCost: </><ManaCost>%.0f</>\n"
						   "<Small>Cooldown: </><Cooldown>%.1f</>"),
						   FMath::Min(Level - 1, this->MaxNumShockTargets), ScaledDamage, Level, ManaCost, Cooldown);
}

void UAuraBeamSpell::StoreMouseDataInfo(const FHitResult& HitResult)
{
	if (HitResult.bBlockingHit)
	{
		this->MouseHitLocation = HitResult.ImpactPoint;
		this->MouseHitActor = HitResult.GetActor();
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void UAuraBeamSpell::StoreOwnerVariables()
{
	if (CurrentActorInfo)
	{
		this->OwnerPlayerController = CurrentActorInfo->PlayerController.Get();
		this->OwningCharacter = Cast<ACharacter>(CurrentActorInfo->AvatarActor);
	}
}

void UAuraBeamSpell::TraceFirstTarget(const FVector& BeamTargetLocation)
{
	check(this->OwningCharacter);
	if (this->OwningCharacter->Implements<UCombatInterface>())
	{
		if (USkeletalMeshComponent* Weapon = ICombatInterface::Execute_GetWeapon(this->OwningCharacter))
		{
			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Add(this->OwningCharacter);
			FHitResult HitResult;
			const FVector SocketLocation = Weapon->GetSocketLocation(FName("TipSocket"));
			UKismetSystemLibrary::SphereTraceSingle(this->OwningCharacter, SocketLocation, BeamTargetLocation, 10.f, TraceTypeQuery1, 
													false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);

			if (HitResult.bBlockingHit)
			{
				this->MouseHitLocation = HitResult.ImpactPoint;
				this->MouseHitActor = HitResult.GetActor();
			}
		}
	}
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(this->MouseHitActor))
	{
		if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UAuraBeamSpell::PrimaryTargetDied))
		{
			CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UAuraBeamSpell::PrimaryTargetDied);
		}
	}
}

void UAuraBeamSpell::StoreAdditionalTargets(TArray<AActor*>& OutAdditionalTargets)
{
	TArray<AActor*> TargetsToIgnore;
	TargetsToIgnore.Add(GetAvatarActorFromActorInfo());
	TargetsToIgnore.Add(this->MouseHitActor);

	TArray<AActor*> Targets;
	UAuraAbilitysystemLibrary::GetLivePlayersWithinRadius(GetAvatarActorFromActorInfo(), Targets, TargetsToIgnore, 850.f, this->MouseHitActor->GetActorLocation());
	
	int32 NumAdditionalTargets = FMath::Min(GetAbilityLevel() - 1, this->MaxNumShockTargets);
	UAuraAbilitysystemLibrary::GetClosestTargets(NumAdditionalTargets, Targets, OutAdditionalTargets, this->MouseHitActor->GetActorLocation());

	for (AActor* Target : OutAdditionalTargets)
	{
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Target))
		{
			if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UAuraBeamSpell::AdditionalTargetDied))
			{
				CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UAuraBeamSpell::AdditionalTargetDied);
			}
		}
	}
}
