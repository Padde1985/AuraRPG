#include "AbilitySystem/Data/LootTiers.h"

// check what items to spawn and how many
TArray<FLootItem> ULootTiers::GetLootItems() const
{
	TArray<FLootItem> ReturnItems;

	for (const FLootItem& Item : this->LootItems)
	{
		for (int32 i = 0; i < Item.MaxNumberToSpawn; i++)
		{
			if (FMath::RandRange(1, 100) < Item.ChanceToSpawn)
			{
				FLootItem NewItem;
				NewItem.LootClass = Item.LootClass;
				NewItem.bLootLevelOverride = Item.bLootLevelOverride;
				ReturnItems.Add(NewItem);
			}
		}
	}

	return ReturnItems;
}
