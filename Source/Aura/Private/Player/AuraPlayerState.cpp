#include "Player/AuraPlayerState.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Net/UnrealNetwork.h"

AAuraPlayerState::AAuraPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");

	// set how often the server wants to send updates
	SetNetUpdateFrequency(100.f);
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return this->AbilitySystemComponent;
}

UAttributeSet* AAuraPlayerState::GetAttributeSet() const
{
	return this->AttributeSet;
}

void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAuraPlayerState, PlayerLevel);
	DOREPLIFETIME(AAuraPlayerState, PlayerXP);
	DOREPLIFETIME(AAuraPlayerState, AttributePoints);
	DOREPLIFETIME(AAuraPlayerState, SpellPoints);
}

int32 AAuraPlayerState::GetPlayerLevel() const
{
	return this->PlayerLevel;
}

void AAuraPlayerState::SetPlayerLevel(int32 Level)
{
	this->PlayerLevel = Level;
	this->OnLevelChangedDelegate.Broadcast(this->PlayerLevel);
}

void AAuraPlayerState::AddToLevel(int32 Level)
{
	this->PlayerLevel += Level;
	this->OnLevelChangedDelegate.Broadcast(this->PlayerLevel);
}

void AAuraPlayerState::AddToXP(int32 XP)
{
	this->PlayerXP += XP;
	this->OnXPChangedDelegate.Broadcast(this->PlayerXP);
}

void AAuraPlayerState::SetXP(int32 XP)
{
	this->PlayerXP = XP;
	this->OnXPChangedDelegate.Broadcast(this->PlayerXP);
}

int32 AAuraPlayerState::GetXP() const
{
	return this->PlayerXP;
}

void AAuraPlayerState::AddToAP(int32 AP)
{
	this->AttributePoints += AP;
	this->OnAttributePointsChangedDelegate.Broadcast(this->AttributePoints);
}

int32 AAuraPlayerState::GetAP() const
{
	return this->AttributePoints;
}

void AAuraPlayerState::AddToSP(int32 SP)
{
	this->SpellPoints += SP;
	this->OnSpellPointsChangedDelegate.Broadcast(this->SpellPoints);
}

int32 AAuraPlayerState::GetSP() const
{
	return this->SpellPoints;
}

void AAuraPlayerState::OnRep_Level(int32 OldLevel)
{
	this->OnLevelChangedDelegate.Broadcast(this->PlayerLevel);
}

void AAuraPlayerState::OnRep_XP(int32 OldXP)
{
	this->OnXPChangedDelegate.Broadcast(this->PlayerXP);
}

void AAuraPlayerState::OnRep_AP(int32 OldAP)
{
	this->OnAttributePointsChangedDelegate.Broadcast(this->AttributePoints);
}

void AAuraPlayerState::OnRep_SP(int32 OldSP)
{
	this->OnSpellPointsChangedDelegate.Broadcast(this->SpellPoints);
}
