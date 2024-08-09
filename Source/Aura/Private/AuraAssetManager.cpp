#include "AuraAssetManager.h"
#include "AuraGameplayTags.h"
#include "AbilitySystemGlobals.h"

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
	// Init global data to receive target data in Ability Tasks
	UAbilitySystemGlobals::Get().InitGlobalData();
}
