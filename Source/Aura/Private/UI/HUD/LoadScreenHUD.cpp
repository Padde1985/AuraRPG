#include "UI/HUD/LoadScreenHUD.h"
#include "Blueprint/UserWidget.h"
#include "UI/Widget/LoadMenuWidget.h"
#include "UI/ViewModel/MVVM_LoadMenu.h"

// generates the Load Screen with Play and Quit buttons
void ALoadScreenHUD::BeginPlay()
{
	Super::BeginPlay();

	this->LoadMenuViewModel = NewObject<UMVVM_LoadMenu>(this, this->LoadMenuViewModelClass);
	this->LoadMenuViewModel->InitializeLoadSlots();

	this->LoadMenuWidget = CreateWidget<ULoadMenuWidget>(GetWorld(), this->LoadMenuWidgetClass);
	this->LoadMenuWidget->AddToViewport();
	this->LoadMenuWidget->BlueprintInitializeWidget();

	this->LoadMenuViewModel->LoadData();
}
