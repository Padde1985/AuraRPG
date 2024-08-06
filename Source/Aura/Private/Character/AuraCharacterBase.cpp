#include "Character/AuraCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"

// Sets default values
AAuraCharacterBase::AAuraCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	this->Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	this->Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	this->Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

FVector AAuraCharacterBase::GetCombatSocketLocation()
{
	check(this->Weapon);
	return this->Weapon->GetSocketLocation(this->WeaponTipSocketName);
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
}

