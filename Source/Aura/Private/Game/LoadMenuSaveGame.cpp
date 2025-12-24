#include "Game/LoadMenuSaveGame.h"

FSavedMap ULoadMenuSaveGame::GetSavedMapByMapName(const FString& InMapName) const
{
	const FSavedMap* FoundMap = this->SavedMaps.FindByPredicate([InMapName](const FSavedMap& Map)
	{
		return Map.MapAssetName == InMapName;
	});

	return FoundMap ? *FoundMap : FSavedMap();
}

bool ULoadMenuSaveGame::HasMap(const FString& InMapName)
{
	return this->SavedMaps.ContainsByPredicate([InMapName](const FSavedMap& Map)
	{
		return Map.MapAssetName == InMapName;
	});
}
