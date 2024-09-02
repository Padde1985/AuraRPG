#include "AuraAssetManager.h"
#include "AuraGameplayTags.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

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
	// fill resistance map
	InitializeStaticTagsToCaptureDefsMap();
	// Init global data to receive target data in Ability Tasks
	UAbilitySystemGlobals::Get().InitGlobalData();
}
