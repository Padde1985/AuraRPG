#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "AuraPlayerState.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
class ULevelInfo;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChanged, int32 /*StatValue*/);

UCLASS()
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly) TObjectPtr<ULevelInfo> LevelInfo;

	AAuraPlayerState();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	int32 GetPlayerLevel() const;
	void SetPlayerLevel(int32 Level);
	void AddToLevel(int32 Level);
	void AddToXP(int32 XP);
	void SetXP(int32 XP);
	int32 GetXP() const;
	FOnPlayerStatChanged OnXPChangedDelegate;
	FOnPlayerStatChanged OnLevelChangedDelegate;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "", meta = (AllowPrivateAccess = "true")) TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "", meta = (AllowPrivateAccess = "true")) TObjectPtr<UAttributeSet> AttributeSet;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "", meta = (AllowPrivateAccess = "true"), ReplicatedUsing = OnRep_Level) int32 PlayerLevel = 1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "", meta = (AllowPrivateAccess = "true"), ReplicatedUsing = OnRep_XP) int32 PlayerXP = 0;


	UFUNCTION() void OnRep_Level(int32 OldLevel);
	UFUNCTION() void OnRep_XP(int32 OldXP);
};
