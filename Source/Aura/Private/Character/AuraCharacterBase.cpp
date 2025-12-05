#include "Character/AuraCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "../Aura.h"
#include "Animation/AnimMontage.h"
#include "AuraGameplayTags.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"

// Sets default values
AAuraCharacterBase::AAuraCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	this->BurnDebuffComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>("Burn Debuff Component");
	this->BurnDebuffComponent->SetupAttachment(GetRootComponent());
	this->BurnDebuffComponent->DebuffTag = FAuraGameplayTags::Get().Debuff_Burn;

	this->StunDebuffComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>("Stun Debuff Component");
	this->StunDebuffComponent->SetupAttachment(GetRootComponent());
	this->StunDebuffComponent->DebuffTag = FAuraGameplayTags::Get().Debuff_Stun;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	this->Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	this->Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	this->Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	this->EffectAttachComponent = CreateDefaultSubobject<USceneComponent>("Effect Attach Point");
	this->EffectAttachComponent->SetupAttachment(GetRootComponent());
	// keep the Component always in the same Orientation -> do not turn with the character
	this->EffectAttachComponent->SetUsingAbsoluteRotation(true);
	this->EffectAttachComponent->SetWorldRotation(FRotator::ZeroRotator);
	this->HaloOfProtectionNiagaraComponent = CreateDefaultSubobject<UPassiveNiagaraComponent>("Halo Of Protection Comp");
	this->HaloOfProtectionNiagaraComponent->SetupAttachment(this->EffectAttachComponent);
	this->LifeSiphonNiagaraComponent = CreateDefaultSubobject<UPassiveNiagaraComponent>("Life Siphon Comp");
	this->LifeSiphonNiagaraComponent->SetupAttachment(this->EffectAttachComponent);
	this->ManaSiphonNiagaraComponent = CreateDefaultSubobject<UPassiveNiagaraComponent>("Mana Siphon Comp");
	this->ManaSiphonNiagaraComponent->SetupAttachment(this->EffectAttachComponent);
}

void AAuraCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAuraCharacterBase, bIsStunned);
	DOREPLIFETIME(AAuraCharacterBase, bIsBurned);
	DOREPLIFETIME(AAuraCharacterBase, bIsBeingShocked);
}

// returns ability system component of character
UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return this->AbilitySystemComponent;
}

// returns the attribute set of the character (Health, Mana, etc.)
UAttributeSet* AAuraCharacterBase::GetAttributeSet() const
{
	return this->AttributeSet;
}

FVector AAuraCharacterBase::GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag)
{
	//TODO: return correct socket based on MontageTag
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_Weapon) && IsValid(this->Weapon))
	{
		return this->Weapon->GetSocketLocation(this->WeaponTipSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_LeftHand))
	{
		return GetMesh()->GetSocketLocation(this->LeftHandSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_RightHand))
	{
		return this->Weapon->GetSocketLocation(this->RightHandSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_Tail))
	{
		return GetMesh()->GetSocketLocation(this->TailSocketName);
	}
	return FVector::ZeroVector;
}

UAnimMontage* AAuraCharacterBase::GetHitReactMontage_Implementation()
{
	return this->HitReactMontage;
}

void AAuraCharacterBase::Die(const FVector& DeathImpulse)
{
	this->Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	this->MulitcastHandleDeath(DeathImpulse);
}

bool AAuraCharacterBase::IsDead_Implementation() const
{
	return this->bIsDead;
}

AActor* AAuraCharacterBase::GetAvatar_Implementation()
{
	return this;
}

TArray<FTaggedMontage> AAuraCharacterBase::GetAttackMontages_Implementation()
{
	return this->AttackMontages;
}

UNiagaraSystem* AAuraCharacterBase::GetBloodEffect_Implementation()
{
	return this->BloodEffect;
}

FTaggedMontage AAuraCharacterBase::GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag)
{
	for (FTaggedMontage TaggedMontage : this->AttackMontages)
	{
		if (TaggedMontage.MontageTag == MontageTag) return TaggedMontage;
	}
	return FTaggedMontage();
}

int32 AAuraCharacterBase::GetMinionCount_Implementation()
{
	return this->MinionCount;
}

void AAuraCharacterBase::UpdateMinionCount_Implementation(int32 Amount)
{
	this->MinionCount += Amount;
}

ECharacterClass AAuraCharacterBase::GetCharacterClass_Implementation()
{

	return this->CharacterClass;
}

FOnASCRegistered& AAuraCharacterBase::GetOnASCRegisteredDelegate()
{
	return this->OnASCRegistered;
}

FOnDeath& AAuraCharacterBase::GetOnDeathDelegate()
{
	return this->OnDeath;
}

void AAuraCharacterBase::Knockback(const FVector& Force)
{
	LaunchCharacter(Force, true, true);
}

USkeletalMeshComponent* AAuraCharacterBase::GetWeapon_Implementation()
{
	return this->Weapon;
}

bool AAuraCharacterBase::IsBeingShocked_Implementation()
{
	return this->bIsBeingShocked;
}

void AAuraCharacterBase::SetIsBeingShocked_Implementation(bool bInShock)
{
	this->bIsBeingShocked = bInShock;
}

float AAuraCharacterBase::TakeDamage(float Damage, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float DamageTaken = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	this->OnDamageDelegate.Broadcast(DamageTaken);

	return DamageTaken;
}

FOnDamageSignature& AAuraCharacterBase::GetOnDamageDelegate()
{
	return this->OnDamageDelegate;
}

void AAuraCharacterBase::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	this->bIsStunned = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = this->bIsStunned ? 0.f : BaseWalkSpeed;
}

void AAuraCharacterBase::OnRep_Stunned()
{
}

void AAuraCharacterBase::OnRep_Burned()
{
}

// _Implementation for Multicast functions required
void AAuraCharacterBase::MulitcastHandleDeath_Implementation(const FVector& DeathImpulse)
{
	UGameplayStatics::PlaySoundAtLocation(this, this->DeathSound, GetActorLocation(), GetActorRotation());

	this->Weapon->SetSimulatePhysics(true);
	this->Weapon->SetEnableGravity(true);
	this->Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	this->Weapon->AddImpulse(DeathImpulse * 0.1f, NAME_None, true); // scale down as weapon is becoming a projectile otherwise

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	GetMesh()->AddImpulse(DeathImpulse, NAME_None, true);
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	this->Dissolve();

	this->bIsDead = true;

	this->BurnDebuffComponent->Deactivate();
	this->StunDebuffComponent->Deactivate();

	this->OnDeath.Broadcast(this);
}

// Called when the game starts or when spawned
void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

// empty implementation, is overridden by child classes
void AAuraCharacterBase::InitAbilityActorInfo()
{
}

// apply effect to this class instance
void AAuraCharacterBase::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffectClass);
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data, GetAbilitySystemComponent());
}

// set default values for all attributes
void AAuraCharacterBase::InitializeDefaultAttributes() const
{
	this->ApplyEffectToSelf(this->DefaultPrimaryAttributes, 1.f);
	this->ApplyEffectToSelf(this->DefaultSecondaryAttributes, 1.f);
	this->ApplyEffectToSelf(this->DefaultVitalAttributes, 1.f);
}

// add starting abilities
void AAuraCharacterBase::AddCharacterAbilities()
{
	if (!HasAuthority()) return;

	UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(this->AbilitySystemComponent);
	AuraASC->AddCharacterAbilities(this->StartupAbilities);
	AuraASC->AddPassiveAbilities(this->StartupPassives);
}

void AAuraCharacterBase::Dissolve()
{
	if (IsValid(this->DissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(this->DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicMatInst);
		this->StartDissolveTimeline(DynamicMatInst);
	}

	if (IsValid(this->WeaponDissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(this->WeaponDissolveMaterialInstance, this);
		this->Weapon->SetMaterial(0, DynamicMatInst);
		this->StartWeaponDissolveTimeline(DynamicMatInst);
	}
}

