#include "Character/AuraEnemy.h"
#include "Aura/Aura.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Components/WidgetComponent.h"
#include "UI/Widget/AuraUserWidget.h"
#include "AbilitySystem/AuraAbilitysystemLibrary.h"
#include "AuraGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AI/AuraAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

AAuraEnemy::AAuraEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");

	this->HealthBar = CreateDefaultSubobject<UWidgetComponent>("Health Bar");
	this->HealthBar->SetupAttachment(GetRootComponent());

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bUseControllerDesiredRotation = true;

	BaseWalkSpeed = 250.f;
}

// highlight actor on mouse over
void AAuraEnemy::HighlightActor()
{
	GetMesh()->SetRenderCustomDepth(true);
	Weapon->SetRenderCustomDepth(true);
}

// remove red frame around actor when mouse over is finished
void AAuraEnemy::UnHighlightActor()
{
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}

// get the enemy level
int32 AAuraEnemy::GetLevel_Implementation()
{
	return this->EnemyLevel;
}

// set custom depth stencil to create the red frame around an enemy
void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = this->BaseWalkSpeed;

	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);

	this->InitAbilityActorInfo();

	if(HasAuthority()) UAuraAbilitysystemLibrary::GiveStartupAbilities(this, AbilitySystemComponent, this->CharacterClass);

	if (UAuraUserWidget* AuraUserWidget = Cast<UAuraUserWidget>(this->HealthBar->GetUserWidgetObject())) AuraUserWidget->SetWidgetController(this);

	if (const UAuraAttributeSet * AS = Cast<UAuraAttributeSet>(AttributeSet))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AS->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			}
		);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AS->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			}
		);
		AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Effects_HitReact, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AAuraEnemy::HitReactTagChanged);

		this->OnHealthChanged.Broadcast(AS->GetHealth());
		this->OnMaxHealthChanged.Broadcast(AS->GetMaxHealth());
	}
}

void AAuraEnemy::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	this->bHitReacting = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = this->bHitReacting ? 0.f : BaseWalkSpeed;
	if(this->AIController && this->AIController->GetBlackboardComponent()) this->AIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), this->bHitReacting);
}

void AAuraEnemy::Die(const FVector& DeathImpulse)
{
	SetLifeSpan(this->LifeSpan);
	if (this->AIController) this->AIController->GetBlackboardComponent()->SetValueAsBool(FName("Dead"), true);

	Super::Die(DeathImpulse);
}

void AAuraEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!HasAuthority()) return;

	this->AIController = Cast<AAuraAIController>(NewController);
	this->AIController->GetBlackboardComponent()->InitializeBlackboard(*this->BehaviorTree->BlackboardAsset);
	this->AIController->RunBehaviorTree(this->BehaviorTree);
	this->AIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), false);
	this->AIController->GetBlackboardComponent()->SetValueAsBool(FName("RangedAttacker"), this->CharacterClass != ECharacterClass::Warrior); // everything besides a warrior is a ranged attacker
}

void AAuraEnemy::SetCombatTarget_Implementation(AActor* InCombatTarget)
{
	this->CombatTarget = InCombatTarget;
}

AActor* AAuraEnemy::GetCombatTarget_Implementation() const
{
	return this->CombatTarget;
}

void AAuraEnemy::Knockback(const FVector& Force)
{
	Super::Knockback(Force);
	this->AIController->StopMovement();
}

// set controll params
void AAuraEnemy::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();

	// do that only on the server
	if(HasAuthority()) InitializeDefaultAttributes();

	OnASCRegistered.Broadcast(AbilitySystemComponent);
	AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Debuff_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AAuraEnemy::StunTagChanged);
}

void AAuraEnemy::InitializeDefaultAttributes() const
{
	UAuraAbilitysystemLibrary::InitializeDefaultAttributes(this->CharacterClass, this->EnemyLevel, this, AbilitySystemComponent);
}

void AAuraEnemy::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	Super::StunTagChanged(CallbackTag, NewCount);

	if (this->AIController && this->AIController->GetBlackboardComponent()) this->AIController->GetBlackboardComponent()->SetValueAsBool(FName("Stunned"), bIsStunned);
}
