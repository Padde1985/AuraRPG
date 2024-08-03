#include "AuraAssetManager.h"
#include "AuraGameplayTags.h"

// AuraAssetManager is set as AssetManager in Config file DefaultEngine.ini and called automatically by the engine
UAuraAssetManager& UAuraAssetManager::Get()
{
	UAuraAssetManager* AuraAssetManager = Cast<UAuraAssetManager>(GEngine->AssetManager);
	return *AuraAssetManager;
}

void UAuraAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	FAuraGameplayTags::InitializeNativeGameplayTags();
}
