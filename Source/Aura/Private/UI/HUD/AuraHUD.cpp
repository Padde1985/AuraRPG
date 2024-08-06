#include "UI/HUD/AuraHUD.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"

// return widget controller for overlay
UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	if (this->WidgetController == nullptr)
	{
		this->WidgetController = NewObject<UOverlayWidgetController>(this, this->OverlayWidgetControllerClass);
		this->WidgetController->SetWidgetControllerParams(WCParams);
		this->WidgetController->BindCallbacksToDependencies();
	}
	return this->WidgetController;
}

// return widget controller for attribute menu
UAttributeMenuWidgetController* AAuraHUD::GetAttributeMenuWidgetController(const FWidgetControllerParams& WCParams)
{
	if (this->AttributeMenuWidgetController == nullptr)
	{
		this->AttributeMenuWidgetController = NewObject<UAttributeMenuWidgetController>(this, this->AttributeMenuWidgetControllerClass);
		this->AttributeMenuWidgetController->SetWidgetControllerParams(WCParams);
		this->AttributeMenuWidgetController->BindCallbacksToDependencies();
	}
	return this->AttributeMenuWidgetController;
}

// create the overall overlay widget
void AAuraHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	checkf(this->OverlayWidgetClass, TEXT("Overlay Widget Class uninitialized, please fill out BP_AuraHUD"));
	checkf(this->OverlayWidgetControllerClass, TEXT("Overlay Widget Class uninitialized, please fill out BP_AuraHUD"));

	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), this->OverlayWidgetClass);
	this->OverlayWidget = Cast<UAuraUserWidget>(Widget);

	const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
	UOverlayWidgetController* OverlayWidgetController = this->GetOverlayWidgetController(WidgetControllerParams);
	this->OverlayWidget->SetWidgetController(OverlayWidgetController);
	OverlayWidgetController->BroadcastInitialValues();

	this->OverlayWidget->AddToViewport();
}
