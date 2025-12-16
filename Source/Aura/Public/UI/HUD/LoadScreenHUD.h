#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LoadScreenHUD.generated.h"

class ULoadMenuWidget;
class UMVVM_LoadMenu;

UCLASS()
class AURA_API ALoadScreenHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly) TSubclassOf<ULoadMenuWidget> LoadMenuWidgetClass;
	UPROPERTY(BlueprintReadOnly) TObjectPtr<ULoadMenuWidget> LoadMenuWidget;
	UPROPERTY(EditDefaultsOnly) TSubclassOf<UMVVM_LoadMenu> LoadMenuViewModelClass;
	UPROPERTY(BlueprintReadOnly) TObjectPtr<UMVVM_LoadMenu> LoadMenuViewModel;

protected:
	virtual void BeginPlay() override;
};
