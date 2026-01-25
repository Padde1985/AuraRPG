#include "Game/LoadMenuSaveGame.h"

// convert user map name to technical map asset name
FSavedMap ULoadMenuSaveGame::GetSavedMapByMapName(const FString& InMapName) const
{
	const FSavedMap* FoundMap = this->SavedMaps.FindByPredicate([InMapName](const FSavedMap& Map)
	{
		return Map.MapAssetName == InMapName;
	});

	return FoundMap ? *FoundMap : FSavedMap();
}

// check if the current map exists in saved maps
bool ULoadMenuSaveGame::HasMap(const FString& InMapName) const
{
	return this->SavedMaps.ContainsByPredicate([InMapName](const FSavedMap& Map)
	{
		return Map.MapAssetName == InMapName;
	});
}
