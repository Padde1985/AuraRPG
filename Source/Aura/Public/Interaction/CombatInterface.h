#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "CombatInterface.generated.h"

class UAnimMontage;
class UNiagaraSystem;
class UAbilitySystemComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnASCRegistered, UAbilitySystemComponent*);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeath, AActor*, DeadActor);

USTRUCT(BlueprintType)
struct FTaggedMontage
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) UAnimMontage* Montage = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FGameplayTag MontageTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FGameplayTag SocketTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) USoundBase* ImpactSound = nullptr;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

class AURA_API ICombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void Die(const FVector& DeathImpulse) = 0;
	virtual FOnASCRegistered& GetOnASCRegisteredDelegate() = 0;
	virtual FOnDeath GetOnDeathDelegate() = 0;
	virtual void Knockback(const FVector& Force) = 0;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) FVector GetCombatSocketLocation(const FGameplayTag& MontageTag);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable) void UpdateWarpTarget(const FVector& FacingTarget);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable) void SetInShockLoop(bool bInLoop);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) UAnimMontage* GetHitReactMontage(); // automatically creates the _implementation function
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) bool IsDead() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) AActor* GetAvatar();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) TArray<FTaggedMontage> GetAttackMontages();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent) UNiagaraSystem* GetBloodEffect();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) FTaggedMontage GetTaggedMontageByTag(const FGameplayTag& MontageTag);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) int32 GetMinionCount();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void UpdateMinionCount(int32 Amount);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) ECharacterClass GetCharacterClass();
	UFUNCTION(BlueprintNativeEvent) int32 GetLevel(); // no pure function (=0), otherwise even the Base class had to implement this function
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) USkeletalMeshComponent* GetWeapon();
};
