#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AuraAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

USTRUCT() struct FEffectProperties
{
	GENERATED_BODY()

	FEffectProperties() {}

	UPROPERTY() UAbilitySystemComponent* SourceASC = nullptr;
	UPROPERTY() UAbilitySystemComponent* TargetASC = nullptr;
	UPROPERTY() AActor* SourceAvatarActor = nullptr;
	UPROPERTY() AActor* TargetAvatarActor = nullptr;
	UPROPERTY() AController* SourceController = nullptr;
	UPROPERTY() AController* TargetController = nullptr;
	UPROPERTY() ACharacter* SourceCharacter = nullptr;
	UPROPERTY() ACharacter* TargetCharacter = nullptr;
	FGameplayEffectContextHandle ContextHandle;
};

UCLASS()
class AURA_API UAuraAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UAuraAttributeSet();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Health);
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Mana);
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxMana);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vital Attributes", meta = (AllowPrivateAccess = "true"), ReplicatedUsing = OnRep_Health) FGameplayAttributeData Health;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vital Attributes", meta = (AllowPrivateAccess = "true"), ReplicatedUsing = OnRep_MaxHealth) FGameplayAttributeData MaxHealth;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vital Attributes", meta = (AllowPrivateAccess = "true"), ReplicatedUsing = OnRep_Mana) FGameplayAttributeData Mana;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vital Attributes", meta = (AllowPrivateAccess = "true"), ReplicatedUsing = OnRep_MaxMana) FGameplayAttributeData MaxMana;

	UFUNCTION() void OnRep_Health(const FGameplayAttributeData& OldHealth) const;
	UFUNCTION() void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;
	UFUNCTION() void OnRep_Mana(const FGameplayAttributeData& OldMana) const;
	UFUNCTION() void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const;
	
	void SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const;
};
