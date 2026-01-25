#include "AbilitySystem/Data/CharacterClassInfo.h"

// get class information for given class
FCharacterClassDefaultsInfo UCharacterClassInfo::GetClassDefaultInfo(ECharacterClass CharacterClass)
{
	return this->CharacterClassInformation.FindChecked(CharacterClass);
}