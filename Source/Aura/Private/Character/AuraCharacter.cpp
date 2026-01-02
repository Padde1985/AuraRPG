#include "Character/AuraCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerState.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Player/AuraPlayerController.h"
#include "UI/HUD/AuraHUD.h"
#include "AbilitySystem/Data/LevelInfo.h"
#include "NiagaraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "Game/AuraGameModeBase.h"
#include "Game/LoadMenuSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraAbilitysystemLibrary.h"
#include "AbilitySystem/Data/AbilityInfo.h"

// set some default values
AAuraCharacter::AAuraCharacter()
{
	this->CameraBoom = CreateDefaultSubobject<USpringArmComponent>("SpringarmComponent");
	this->CameraBoom->SetupAttachment(GetRootComponent());
	this->CameraBoom->SetUsingAbsoluteRotation(true);
	this->CameraBoom->bDoCollisionTest = false;

	this->CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	this->CameraComponent->SetupAttachment(this->CameraBoom, USpringArmComponent::SocketName);
	this->CameraComponent->bUsePawnControlRotation = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	CharacterClass = ECharacterClass::Elementalist;

	this->LevelUpNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("LevelUpNiagaraComponent");
	this->LevelUpNiagaraComponent->SetupAttachment(GetRootComponent());
	this->LevelUpNiagaraComponent->bAutoActivate = false;
}

// called by the replication system for the server
void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Init Ability actor info for the server
	this->InitAbilityActorInfo();
	this->LoadProgress();

	if (AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		GameMode->LoadWorldState(GetWorld());
	}
}

// get the player level for ability value calculation
int32 AAuraCharacter::GetLevel_Implementation()
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetPlayerLevel();
}

void AAuraCharacter::AddToXP_Implementation(int32 InXP)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddToXP(InXP);
}

void AAuraCharacter::LevelUp_Implementation()
{
	this->MulticastLevelUpParticles();
}

int32 AAuraCharacter::GetXP_Implementation() const
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetXP();	
}

int32 AAuraCharacter::FindLevelForXP_Implementation(int32 InXP) const
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->LevelInfo->FindLevelForXP(InXP);
}

int32 AAuraCharacter::GetAttributePointsReward_Implementation(int32 Level) const
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->LevelInfo->LevelInformation[Level].AttributePoints;
}

int32 AAuraCharacter::GetAttributePoints_Implementation() const
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetAP();
}

int32 AAuraCharacter::GetSpellPointsReward_Implementation(int32 Level) const
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->LevelInfo->LevelInformation[Level].SpellPoints;
}

int32 AAuraCharacter::GetSpellPoints_Implementation() const
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetSP();
}

void AAuraCharacter::AddToAttributePoints_Implementation(int32 InPoints)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddToAP(InPoints);
}

void AAuraCharacter::AddToSpellPoints_Implementation(int32 InPoints)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddToSP(InPoints);
}

void AAuraCharacter::AddToPlayerLevel_Implementation(int32 inPlayerLevel)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddToLevel(inPlayerLevel);

	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(this->GetAbilitySystemComponent()))
	{
		AuraASC->UpdateAbilityStatuses(AuraPlayerState->GetPlayerLevel());
	}
}

void AAuraCharacter::ShowMagicCircle_Implementation(UMaterialInterface* DecalMaterial)
{
	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		AuraPlayerController->ShowMagicCircle(DecalMaterial);
		AuraPlayerController->bShowMouseCursor = false;
	}
}

void AAuraCharacter::HideMagicCircle_Implementation()
{
	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		AuraPlayerController->HideMagicCircle();
		AuraPlayerController->bShowMouseCursor = true;
	}
}

void AAuraCharacter::SaveProgress_Implementation(const FName& CheckpointTag)
{
	if (AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		ULoadMenuSaveGame* SaveData = GameMode->RetrieveInGameSaveData();
		if (SaveData == nullptr) return;

		SaveData->PlayerStartTag = CheckpointTag;
		if (AAuraPlayerState* SavePlayerState = Cast<AAuraPlayerState>(GetPlayerState()))
		{
			SaveData->PlayerLevel = SavePlayerState->GetPlayerLevel();
			SaveData->XP = SavePlayerState->GetXP();
			SaveData->AttributePoints = SavePlayerState->GetAP();
			SaveData->SpellPoints = SavePlayerState->GetSP();
		}
		// Get Base values instead of the current values -> ignores any current buffs or debuffs
		SaveData->Strength = UAuraAttributeSet::GetStrengthAttribute().GetGameplayAttributeData(GetAttributeSet())->GetBaseValue();
		SaveData->Intelligence = UAuraAttributeSet::GetIntelligenceAttribute().GetGameplayAttributeData(GetAttributeSet())->GetBaseValue();
		SaveData->Resilience = UAuraAttributeSet::GetResilienceAttribute().GetGameplayAttributeData(GetAttributeSet())->GetBaseValue();
		SaveData->Vigor = UAuraAttributeSet::GetVigorAttribute().GetGameplayAttributeData(GetAttributeSet())->GetBaseValue();
			
		SaveData->bFirstTimeLoadIn = false;

		if (!HasAuthority()) return;

		UAuraAbilitySystemComponent* ASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);
		FForeachAbility SavedAbilityDelegate;
		SaveData->SavedAbilities.Empty();
		SavedAbilityDelegate.BindLambda([this, ASC, SaveData](const FGameplayAbilitySpec& AbilitySpec)
		{
			FSavedAbility SavedAbility;
			const FGameplayTag AbilityTag = ASC->GetAbilityTagFromSpec(AbilitySpec);
			FAuraAbilityInfo Info = UAuraAbilitysystemLibrary::GetAbilityInfo(this)->FindAbilityInfoForTag(AbilityTag);

			SavedAbility.GameplayAbility = Info.Ability;
			SavedAbility.Level = AbilitySpec.Level;
			SavedAbility.AbilitySlot = ASC->GetSlotFromAbilityTag(AbilityTag);
			SavedAbility.AbilityStatus = ASC->GetStatusFromAbilityTag(AbilityTag);
			SavedAbility.AbilityTag = AbilityTag;
			SavedAbility.AbilityType = Info.AbilityType;

			SaveData->SavedAbilities.AddUnique(SavedAbility);
		});
		ASC->ForEachAbility(SavedAbilityDelegate);

		GameMode->SaveInGameProgressData(SaveData);
	}
}

void AAuraCharacter::Die(const FVector& DeathImpulse)
{
	Super::Die(DeathImpulse);

	FTimerDelegate DeathTimerDelegate;
	DeathTimerDelegate.BindLambda([this]()
	{
		if (AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			GameMode->PlayerDied(this);
		}
	});
	GetWorldTimerManager().SetTimer(DeathTimer, DeathTimerDelegate, this->DeathTime, false);
	this->CameraComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
}

void AAuraCharacter::MulticastLevelUpParticles_Implementation() const
{
	if (IsValid(this->LevelUpNiagaraComponent))
	{
		const FVector CameraLocation = this->CameraComponent->GetComponentLocation();
		const FVector NiagaraSystemLocation = this->LevelUpNiagaraComponent->GetComponentLocation();
		const FRotator ToCameraRotation = (CameraLocation - NiagaraSystemLocation).Rotation();

		this->LevelUpNiagaraComponent->SetWorldRotation(ToCameraRotation);
		this->LevelUpNiagaraComponent->Activate(true);
	}
}

// called by the replication system for the client
void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Init Ability actor info for the client
	this->InitAbilityActorInfo();
}

// initialize default values when starting the game
void AAuraCharacter::InitAbilityActorInfo()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState, this);
	Cast<UAuraAbilitySystemComponent>(AuraPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
	AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
	AttributeSet = AuraPlayerState->GetAttributeSet();
	OnASCRegistered.Broadcast(AbilitySystemComponent);
	AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Debuff_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AAuraCharacter::StunTagChanged);

	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(AuraPlayerController->GetHUD()))
		{
			AuraHUD->InitOverlay(AuraPlayerController, AuraPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
}

void AAuraCharacter::OnRep_Stunned()
{
	if (UAuraAbilitySystemComponent* ASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		FGameplayTagContainer BlockedTags;
		const FAuraGameplayTags& StunTags = FAuraGameplayTags::Get();
		BlockedTags.AddTag(StunTags.Player_Block_CursorTrace);
		BlockedTags.AddTag(StunTags.Player_Block_InputHeld);
		BlockedTags.AddTag(StunTags.Player_Block_InputPressed);
		BlockedTags.AddTag(StunTags.Player_Block_InputReleased);
		if (bIsStunned)
		{
			ASC->AddLooseGameplayTags(BlockedTags);
			StunDebuffComponent->Activate();
		}
		else
		{
			ASC->RemoveLooseGameplayTags(BlockedTags);
			StunDebuffComponent->Deactivate();
		}
	}
}

void AAuraCharacter::OnRep_Burned()
{
	if (bIsBurned)
	{
		BurnDebuffComponent->Activate();
	}
	else
	{
		BurnDebuffComponent->Deactivate();
	}
}

void AAuraCharacter::LoadProgress()
{
	if (AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		ULoadMenuSaveGame* SaveData = GameMode->RetrieveInGameSaveData();
		if (SaveData == nullptr) return;

		if (SaveData->bFirstTimeLoadIn)
		{
			InitializeDefaultAttributes();
			AddCharacterAbilities();
		}
		else
		{
			if (UAuraAbilitySystemComponent* ASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
			{
				ASC->AddCharacterAbilitiesFromSaveData(SaveData);
				ASC->UpdateAbilityStatuses(SaveData->PlayerLevel); //update eligible abilities
			}

			if (AAuraPlayerState* AuraPlayerState = Cast<AAuraPlayerState>(GetPlayerState()))
			{
				AuraPlayerState->SetPlayerLevel(SaveData->PlayerLevel);
				AuraPlayerState->SetXP(SaveData->XP);
				AuraPlayerState->SetAP(SaveData->AttributePoints);
				AuraPlayerState->SetSP(SaveData->SpellPoints);
			}

			UAuraAbilitysystemLibrary::InitializeDefaultAttributesFromSaveGame(this, AbilitySystemComponent, SaveData);
		}
	}
}