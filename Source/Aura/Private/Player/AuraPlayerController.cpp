#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/EnemyInterface.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "Components/SplineComponent.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

// enable replication for mulitplayer
AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;

	this->Spline = CreateDefaultSubobject<USplineComponent>("Spline"); // path finding
}

// set some default values like mouse cursor behavior
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

// set input component to move character and start abilities
void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	AuraInputComponent->BindAction(this->MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	AuraInputComponent->BindAbilityActions(this->InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

// trace for highlighting enemies
void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	this->CursorTrace();
	this->AutoRun();
}

// move character with keys
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

// trace object under cursor and check what has to be highlighted and unhighlighted
void AAuraPlayerController::CursorTrace()
{
	GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, this->CursorHit);
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
		if (this->LastActor != this->ThisActor)
		{
			if (this->LastActor) this->LastActor->UnHighlightActor();
			if (this->ThisActor) this->ThisActor->HighlightActor();
		}
	}
}

// Ability was just pressed and released immediately
void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if(InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		this->bTargeting = this->ThisActor ? true : false;
		this->bAutoRunning = false;
	}
}

// Ability button was released
void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (this->GetASC())	this->GetASC()->AbilityInputTagReleased(InputTag);
		return;
	}

	if (this->bTargeting)
	{
		if (this->GetASC())	this->GetASC()->AbilityInputTagReleased(InputTag);
	}
	// configuring auto run
	else
	{
		APawn* ControlledPawn = GetPawn();
		if (this->FollowTime <= this->ShortPressThreshold && ControlledPawn)
		{
			if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(), this->CachedDestination))
			{
				this->Spline->ClearSplinePoints();
				for (const FVector& PointLoc : NavPath->PathPoints)
				{
					this->Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
				}
				// reset Cached Destination to avoid infinite auro running when clicking on an area outside of the Nav Mesh volume
				this->CachedDestination = NavPath->PathPoints[NavPath->PathPoints.Num() - 1];
				this->bAutoRunning = true;
			}
		}
		this->FollowTime = 0.f;
		this->bTargeting = false;
	}
}

// Ability button is held down
void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	// handle all inputs but the LMB
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (this->GetASC())	this->GetASC()->AbilityInputTagHeld(InputTag);
		return;
	}
	
	// otherwise check if mouse cursor is over an enemy
	if (this->bTargeting)
	{
		if (this->GetASC())	this->GetASC()->AbilityInputTagHeld(InputTag);
	}
	// otherwise move the player to the mouse cursor point
	else
	{
		this->FollowTime += GetWorld()->GetDeltaSeconds();
		if (this->CursorHit.bBlockingHit) this->CachedDestination = this->CursorHit.Location;

		if (APawn* ControlledPawn = GetPawn())
		{
			const FVector WorldDirection = (this->CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection);
		}
	}
}

// get ability system component to avoid multiple castings
UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if (this->AuraAbilitySystemComponent == nullptr) 
		this->AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));

	return this->AuraAbilitySystemComponent;
}

void AAuraPlayerController::AutoRun()
{
	if (!this->bAutoRunning) return;

	if (APawn* ControlledPawn = GetPawn())
	{
		const FVector LocationOnSpline = this->Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		const FVector Direction = this->Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
		ControlledPawn->AddMovementInput(Direction);

		const float DistanceToDestination = (LocationOnSpline - this->CachedDestination).Length();
		if (DistanceToDestination <= this->AutoRunAcceptanceRadius) this->bAutoRunning = false;
	}
}
