#include "AbilitySystem/Abilities/AuraArcaneShards.h"

FString UAuraArcaneShards::GetDescription(int32 Level)
{
	// get scaled damage from curve table for a given level
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);

	if (Level == 1)
	{
		return FString::Printf(TEXT("<Title>ARCANE SHARDS</>\n\n"
							   "<Default>Summon one Arcane Shard dealing </><Damage>%d </>"
							   "<Default>arcane damage with knockback</>\n\n"
							   "<Small>Level: </><Level>%d</>\n"
							   "<Small>ManaCost: </><ManaCost>%.0f</>\n"
							   "<Small>Cooldown: </><Cooldown>%.1f</>"),
							   ScaledDamage, Level, ManaCost, Cooldown);
	}
	else
	{
		return FString::Printf(TEXT("<Title>ARCANE SHARDS</>\n\n"
							   "<Default>Summon %d Arcane Shards dealing </><Damage>%d </>"
							   "<Default>arcane damage with knockback</>\n\n"
							   "<Small>Level: </><Level>%d</>\n"
							   "<Small>ManaCost: </><ManaCost>%.0f</>\n"
							   "<Small>Cooldown: </><Cooldown>%.1f</>"),
							   FMath::Min(Level, this->MaxNumShards), ScaledDamage, Level, ManaCost, Cooldown);
	}
}

FString UAuraArcaneShards::GetNextLevelDescription(int32 Level)
{
	// get scaled damage from curve table for a given level
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);

	return FString::Printf(TEXT("<Title>NEXT LEVEL</>\n\n"
						   "<Default>Summon %d Arcane Shards dealing </><Damage>%d </>"
						   "<Default>arcane damage with knockback</>\n\n"
						   "<Small>Level: </><Level>%d</>\n"
						   "<Small>ManaCost: </><ManaCost>%.0f</>\n"
						   "<Small>Cooldown: </><Cooldown>%.1f</>"),
						   FMath::Min(Level, this->MaxNumShards), ScaledDamage, Level, ManaCost, Cooldown);
}
