#include "AbilitySystem/Data/CharacterClassInfo.h"

FCharacterClassDefaultsInfo UCharacterClassInfo::GetClassDefaultInfo(ECharacterClass CharacterClass)
{
	return this->CharacterClassInformation.FindChecked(CharacterClass);
}
