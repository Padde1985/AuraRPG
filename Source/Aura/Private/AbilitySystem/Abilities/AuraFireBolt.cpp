#include "AbilitySystem/Abilities/AuraFireBolt.h"
#include "AuraGameplayTags.h"

FString UAuraFireBolt::GetDescription(int32 Level)
{
	const int32 Damage = GetDamageByDamageType(Level, FAuraGameplayTags::Get().Damage_Fire);
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);

	if (Level == 1)
	{
		return FString::Printf(TEXT("<Title>FIRE BOLT</>\n\n"
										"<Default>Launches %d bolt of fire, exploding on impact and dealing </><Damage>%d </>"
										"<Default>fire damage with a chance to burn</>\n\n"
										"<Small>Level: </><Level>%d</>\n"
										"<Small>ManaCost: </><ManaCost>%.0f</>\n"
										"<Small>Cooldown: </><Cooldown>%.1f</>"),
									Level, Damage, Level, ManaCost, Cooldown);
	}
	else
	{
		return FString::Printf(TEXT("<Title>FIRE BOLT</>\n\n"
										"<Default>Launches %d bolts of fire, exploding on impact and dealing </><Damage>%d </>"
										"<Default>fire damage with a chance to burn</>\n\n"
										"<Small>Level: </><Level>%d</>\n"
										"<Small>ManaCost: </><ManaCost>%.0f</>\n"
										"<Small>Cooldown: </><Cooldown>%.1f</>"),
									FMath::Min(Level, this->NumProjectiles), Damage, Level, ManaCost, Cooldown);
	}
}

FString UAuraFireBolt::GetNextLevelDescription(int32 Level)
{
	const int32 Damage = GetDamageByDamageType(Level, FAuraGameplayTags::Get().Damage_Fire);
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);

	return FString::Printf(TEXT("<Title>Next Level</>\n\n"
									"<Default>Launches %d bolts of fire, exploding on impact and dealing </><Damage>%d </>"
									"<Default>fire damage with a chance to burn</>\n\n"
									"<Small>Level: </><Level>%d</>\n"
									"<Small>ManaCost: </><ManaCost>%.0f</>\n"
									"<Small>Cooldown: </><Cooldown>%.1f</>"),
								FMath::Min(Level, this->NumProjectiles), Damage, Level, ManaCost, Cooldown);
}
