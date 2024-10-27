#include "AbilitySystem/Data/LevelInfo.h"

int32 ULevelInfo::FindLevelForXP(int32 XP) const
{
	for (FAuraLevelInfo Info : this->LevelInformation)
	{
		if (Info.LevelUpRequirement >= XP && XP > 0) return Info.Level;
	}

	return 1;
}

FAuraLevelInfo ULevelInfo::GetInfoForLevel(int32 Level) const
{
	// try to find exact Level
	for (FAuraLevelInfo Info : this->LevelInformation)
	{
		if (Info.Level == Level) return Info;
	}

	// if Level not existing, get index of next higher one and return info of previous level
	for (int i = 0; i < this->LevelInformation.Num() - 1; i++)
	{
		if (this->LevelInformation[i].Level > Level)
		{
			if (i > 0)
			{
				return this->LevelInformation[i - 1];
			}
			else
			{
				return this->LevelInformation[i];
			}
		}
	}
	return FAuraLevelInfo();
}
