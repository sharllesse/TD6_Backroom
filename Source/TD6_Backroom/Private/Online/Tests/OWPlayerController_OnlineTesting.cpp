// Copyright (c) Logicraft Interactive. All Rights Reserved.

#include "Online/Tests/OWPlayerController_OnlineTesting.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "EventBus.h"
#include "Online/OWOnlineGameTags.h"
#include "Online/OWOnlineSubsystem.h"

void AOWPlayerController_OnlineTesting::BeginPlay()
{
	Super::BeginPlay();

	UEventBus::AddUObject(this, Online_Callback_OnExternalUIChange, this, &AOWPlayerController_OnlineTesting::OnExternalUIChange);
}

void AOWPlayerController_OnlineTesting::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UEventBus::RemoveAll(this, Online_Callback_OnExternalUIChange, this);
}

void AOWPlayerController_OnlineTesting::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(InputComponent))
	{
		for (FAction& InputAction : InputActions)
		{
			Input->BindAction(InputAction.Action, InputAction.TriggerEvent, this, InputAction.FunctionName);
		}

		return;
	}
	
	UE_LOG(LogTemp, Error, TEXT("AFPSController: The input component is null."))
}

void AOWPlayerController_OnlineTesting::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	OwningCharacter = Cast<AOWPlayerCharacter_OnlineTesting>(InPawn);

	if (IsLocalPlayerController())
	{
		SetupInputMappingContext();
	}
}

void AOWPlayerController_OnlineTesting::OnRep_Pawn()
{
	Super::OnRep_Pawn();

	OwningCharacter = Cast<AOWPlayerCharacter_OnlineTesting>(GetPawn());
	
	SetupInputMappingContext();	
}

void AOWPlayerController_OnlineTesting::SetupInputMappingContext()
{
	if (ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player))
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				InputSystem->AddMappingContext(InputMappingContext, 0);
				return;
			}

			UE_LOG(LogTemp, Error, TEXT("AFPSController: The mapping context cannot be null."))
		}
	}
}

void AOWPlayerController_OnlineTesting::OnMove(const FInputActionValue& InputActionValue)
{
	OwningCharacter->OnMove(InputActionValue);
}

void AOWPlayerController_OnlineTesting::OnLook(const FInputActionValue& InputActionValue)
{
	const FVector2D LookInput{InputActionValue.Get<FVector2D>()};
	
	AddPitchInput(LookInput.Y);
	AddYawInput(LookInput.X);
}

void AOWPlayerController_OnlineTesting::OnJump(const FInputActionValue& InputActionValue)
{
	OwningCharacter->OnJump(InputActionValue);
}

void AOWPlayerController_OnlineTesting::OnCreateSession_Debug()
{
	UOWOnlineSubsystem::Get(GetWorld())->CreateSession();
}	

void AOWPlayerController_OnlineTesting::OnExternalUIChange(bool bIsOpening)
{
	FlushPressedKeys();
	
	if (bIsOpening)
	{
		bShowMouseCursor = true;
		bEnableClickEvents = true;
		bEnableMouseOverEvents = true;

		FInputModeUIOnly InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(InputMode);
	}
	else
	{
		bShowMouseCursor = false;
		bEnableClickEvents = false;
		bEnableMouseOverEvents = false;

		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
	}
}
