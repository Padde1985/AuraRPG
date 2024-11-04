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

//typedef TBaseStaticDelegateInstance<FGameplayAttribute(), FDefaultDelegateUserPolicy>::FFuncPtr FAttributeFuncPtr; <- "old" C++ syntax specific for FGameplayAttribute() signature
template<class T> using TStaticFuncPtr = typename TBaseStaticDelegateInstance<T, FDefaultDelegateUserPolicy>::FFuncPtr; // <- new modern style to accept any type of function type signature
//example:
// TStaticFuncPtr,float(int32, float, int32)> RandomFunctionPointer;
// float RandomFunction(int32 I, float F, int32 I2) { return 0.f; }
// RandomFunctionPointer = RandomFunction
// float F = RandomFunctionPointer(0.f, 0.f, 0);

UCLASS()
class AURA_API UAuraAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	TMap<FGameplayTag, TStaticFuncPtr<FGameplayAttribute()>> TagsToAttributes;
	// alternative way to write that:
	//TMap<FGameplayTag, FGameplayAttribute(*)()> TagsToAttributes;

	UAuraAttributeSet();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Health);
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Mana);
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxMana);
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Strength);
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Intelligence);
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Resilience);
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Vigor);
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Armor);
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, ArmorPenetration);
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, BlockChance);
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, CritChance);
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, CritDamage);
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, CritResistance);
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, HealthRegeneration);
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, ManaRegeneration);
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, IncomingDamage);
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, IncomingXP);
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, FireResistance);
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, LightningResistance);
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, ArcaneResistance);
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, PhysicalResistance);

	/*
	* Vital Attributes
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vital Attributes", ReplicatedUsing = OnRep_Health) FGameplayAttributeData Health;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vital Attributes", ReplicatedUsing = OnRep_Mana) FGameplayAttributeData Mana;

	/*
	* Meta Attributes
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Meta Attributes") FGameplayAttributeData IncomingDamage;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Meta Attributes") FGameplayAttributeData IncomingXP;

	/*
	* Primary Attributes
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Primary Attributes", ReplicatedUsing = OnRep_Strength) FGameplayAttributeData Strength;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Primary Attributes", ReplicatedUsing = OnRep_Intelligence) FGameplayAttributeData Intelligence;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Primary Attributes", ReplicatedUsing = OnRep_Resilience) FGameplayAttributeData Resilience;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Primary Attributes", ReplicatedUsing = OnRep_Vigor) FGameplayAttributeData Vigor;

	/*
	* Secondary Attributes
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Secondary Attributes", ReplicatedUsing = OnRep_Armor) FGameplayAttributeData Armor;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Secondary Attributes", ReplicatedUsing = OnRep_ArmorPenetration) FGameplayAttributeData ArmorPenetration;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Secondary Attributes", ReplicatedUsing = OnRep_BlockChance) FGameplayAttributeData BlockChance;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Secondary Attributes", ReplicatedUsing = OnRep_CritChance) FGameplayAttributeData CritChance;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Secondary Attributes", ReplicatedUsing = OnRep_CritDamage) FGameplayAttributeData CritDamage;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Secondary Attributes", ReplicatedUsing = OnRep_CritResistance) FGameplayAttributeData CritResistance;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Secondary Attributes", ReplicatedUsing = OnRep_HealthRegeneration) FGameplayAttributeData HealthRegeneration;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Secondary Attributes", ReplicatedUsing = OnRep_ManaRegeneration) FGameplayAttributeData ManaRegeneration;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Secondary Attributes", ReplicatedUsing = OnRep_MaxHealth) FGameplayAttributeData MaxHealth;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Secondary Attributes", ReplicatedUsing = OnRep_MaxMana) FGameplayAttributeData MaxMana;

	/*
	* Resistances
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resistance Attributes", ReplicatedUsing = OnRep_FireResistance) FGameplayAttributeData FireResistance;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resistance Attributes", ReplicatedUsing = OnRep_LightningResistance) FGameplayAttributeData LightningResistance;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resistance Attributes", ReplicatedUsing = OnRep_ArcaneResistance) FGameplayAttributeData ArcaneResistance;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resistance Attributes", ReplicatedUsing = OnRep_PhysicalResistance) FGameplayAttributeData PhysicalResistance;

	UFUNCTION() void OnRep_Health(const FGameplayAttributeData& OldHealth) const;
	UFUNCTION() void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;
	UFUNCTION() void OnRep_Mana(const FGameplayAttributeData& OldMana) const;
	UFUNCTION() void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const;
	UFUNCTION() void OnRep_Strength(const FGameplayAttributeData& OldStrength) const;
	UFUNCTION() void OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const;
	UFUNCTION() void OnRep_Resilience(const FGameplayAttributeData& OldResilience) const;
	UFUNCTION() void OnRep_Vigor(const FGameplayAttributeData& OldVigor) const;
	UFUNCTION() void OnRep_Armor(const FGameplayAttributeData& OldArmor) const;
	UFUNCTION() void OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const;
	UFUNCTION() void OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const;
	UFUNCTION() void OnRep_CritChance(const FGameplayAttributeData& OldCritChance) const;
	UFUNCTION() void OnRep_CritDamage(const FGameplayAttributeData& OldCritDamage) const;
	UFUNCTION() void OnRep_CritResistance(const FGameplayAttributeData& OldCritResistance) const;
	UFUNCTION() void OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const;
	UFUNCTION() void OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const;
	UFUNCTION() void OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance) const;
	UFUNCTION() void OnRep_LightningResistance(const FGameplayAttributeData& OldLightningResistance) const;
	UFUNCTION() void OnRep_ArcaneResistance(const FGameplayAttributeData& OldArcaneResistance) const;
	UFUNCTION() void OnRep_PhysicalResistance(const FGameplayAttributeData& OldPhysicalResistance) const;
	
private:
	bool bTopOffHealth = false;
	bool bTopOffMana = false;

	void SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const;
	void ShowFloatingText(const FEffectProperties& Props, float Damage, bool bIsBlocked, bool bIsCritical) const;
	void SendXPEvent(const FEffectProperties& Props);
};
