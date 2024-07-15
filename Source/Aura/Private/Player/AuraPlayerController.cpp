#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Interaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(this->AuraContext);

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(this->AuraContext, 0);
	}

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(this->MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	this->CursorTrace();
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.X);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.Y);
	}
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, CursorHit);
	if (CursorHit.bBlockingHit)
	{
		this->LastActor = this->ThisActor;
		this->ThisActor = CursorHit.GetActor(); // TScriptInterface takes care of the cast for us

		/*
		Line trace from cursor. There are several scenarios
		1. LastActor is null && ThisActor is null
			- Do nothing
		2. LastActor is null && ThisActor is valid
			- HighLight ThisActor
		3. LastActor is valid && ThisActor is null
			- UnHighlight LastActor
		4. LastActor is valid && ThisActor is valid && LastActor != ThisActor
			- UnHighlight LastActor
			- Highlight ThisActor
		5. LastActor is valid && ThisActor is valid && LastActor == ThisActor
			- Do nothing
		*/
		if (!this->LastActor)
		{
			if (this->ThisActor)
			{
				// Case 2
				this->ThisActor->HighlightActor();
			}
			else
			{
				// Case 1 -> do nothing
			}
		}
		else // cases 3-5
		{
			if (!this->ThisActor)
			{
				// Case 3
				this->LastActor->UnHighlightActor();
			}
			else
			{
				if (this->ThisActor != this->LastActor)
				{
					// Case 4
					this->LastActor->UnHighlightActor();
					this->ThisActor->HighlightActor();
				}
				else
				{
					// Case 5 -> do nothing
				}
			}
		}
	}
}
