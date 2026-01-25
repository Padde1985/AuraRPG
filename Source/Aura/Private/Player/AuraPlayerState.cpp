#include "Player/AuraPlayerState.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Net/UnrealNetwork.h"

// constructing the player state
AAuraPlayerState::AAuraPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");

	// set how often the server wants to send updates
	SetNetUpdateFrequency(100.f);
}

// getter for ASC
UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return this->AbilitySystemComponent;
}

// getter for AS
UAttributeSet* AAuraPlayerState::GetAttributeSet() const
{
	return this->AttributeSet;
}

// set replication for variables, only needed for multiplayer
void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAuraPlayerState, PlayerLevel);
	DOREPLIFETIME(AAuraPlayerState, PlayerXP);
	DOREPLIFETIME(AAuraPlayerState, AttributePoints);
	DOREPLIFETIME(AAuraPlayerState, SpellPoints);
}

// getter for Player level
int32 AAuraPlayerState::GetPlayerLevel() const
{
	return this->PlayerLevel;
}

// setter for Player level
void AAuraPlayerState::SetPlayerLevel(int32 Level)
{
	this->PlayerLevel = Level;
	this->OnLevelChangedDelegate.Broadcast(this->PlayerLevel, false);
}

// changing player level and broadcasting change to update widgets
void AAuraPlayerState::AddToLevel(int32 Level)
{
	this->PlayerLevel += Level;
	this->OnLevelChangedDelegate.Broadcast(this->PlayerLevel, true);
}

// changing player XP and broadcasting to update widgets
void AAuraPlayerState::AddToXP(int32 XP)
{
	this->PlayerXP += XP;
	this->OnXPChangedDelegate.Broadcast(this->PlayerXP);
}


// set initial XP (or XP loaded from save game) and broadcast value to widgets
void AAuraPlayerState::SetXP(int32 XP)
{
	this->PlayerXP = XP;
	this->OnXPChangedDelegate.Broadcast(this->PlayerXP);
}

// getter for XP
int32 AAuraPlayerState::GetXP() const
{
	return this->PlayerXP;
}

// changing AP and broadcasting to update widgets
void AAuraPlayerState::AddToAP(int32 AP)
{
	this->AttributePoints += AP;
	this->OnAttributePointsChangedDelegate.Broadcast(this->AttributePoints);
}

// getter for AP
int32 AAuraPlayerState::GetAP() const
{
	return this->AttributePoints;
}

//setter for AP
void AAuraPlayerState::SetAP(int32 InAP)
{
	this->AttributePoints = InAP;
	this->OnAttributePointsChangedDelegate.Broadcast(this->AttributePoints);
}

// change SP and brodacast to update widgets
void AAuraPlayerState::AddToSP(int32 SP)
{
	this->SpellPoints += SP;
	this->OnSpellPointsChangedDelegate.Broadcast(this->SpellPoints);
}

// getter for SP
int32 AAuraPlayerState::GetSP() const
{
	return this->SpellPoints;
}

// setter for SP
void AAuraPlayerState::SetSP(int32 InSP)
{
	this->SpellPoints = InSP;
	this->OnSpellPointsChangedDelegate.Broadcast(this->SpellPoints);
}

// replication function to update Player level
void AAuraPlayerState::OnRep_Level(int32 OldLevel)
{
	this->OnLevelChangedDelegate.Broadcast(this->PlayerLevel, true);
}

// replication function to update player XP
void AAuraPlayerState::OnRep_XP(int32 OldXP)
{
	this->OnXPChangedDelegate.Broadcast(this->PlayerXP);
}

// replication function to update AP
void AAuraPlayerState::OnRep_AP(int32 OldAP)
{
	this->OnAttributePointsChangedDelegate.Broadcast(this->AttributePoints);
}

// replication function to update SP
void AAuraPlayerState::OnRep_SP(int32 OldSP)
{
	this->OnSpellPointsChangedDelegate.Broadcast(this->SpellPoints);
}