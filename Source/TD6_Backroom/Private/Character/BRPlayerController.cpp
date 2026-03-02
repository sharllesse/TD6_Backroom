// Copyright (c) Logicraft Interactive. All Rights Reserved.

#include "Character/BRPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "EventBus.h"
#include "GameMode/BRGameModeGameplayTags.h"
#include "GameState/BRGameStateGameplayTags.h"
#include "Online/BROnlineGameTags.h"
#include "UI/UIManagerSubsystem.h"
#include "UI/InGameUI.h"


void ABRPlayerController::BeginPlay()
{
	Super::BeginPlay();
	 
	

	if (IsLocalController())
	{
		SetupLocalInfo();
	}
	


	UEventBus::AddLambda(this, GameState_Callback_OnObjectivesCompleted, []
	{
		GEngine->AddOnScreenDebugMessage(0, 2.f, FColor::Red, TEXT("All vhs collected."));
	});

	UEventBus::AddLambda(this, GameMode_Callback_OnAllPlayerInExitZone, []
		{
			GEngine->AddOnScreenDebugMessage(0, 2.f, FColor::Red, TEXT("All player in exit zone."));
		});
	
	
	//UEventBus::AddUObject(this, Online_Callback_OnExternalUIChange, this, &ABRPlayerController::OnExternalUIChange);
}

void ABRPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

}

void ABRPlayerController::SetupLocalInfo()
{
	SetInputMode(FInputModeGameOnly());
	bShowMouseCursor = false; 
	
		
	UEventBus::AddLambda(this, Online_Callback_OnMainExternalUIOverlayChange, [this](bool bIsOpening)
	{
		if (bIsOpening)
		{
			SetInputMode(FInputModeUIOnly());
			bShowMouseCursor = true; 
		}
		else
		{
			SetInputMode(FInputModeGameOnly());
			bShowMouseCursor = false; 
		}
	});

	InGameUI = GetLocalPlayer()->GetSubsystem<UUIManagerSubsystem>()->CreateWidget<UInGameUI>();
	OwningCharacter->SetNotifyInteractCallback([this](bool bCanInteract,const IInteractable* Interactable)
	{
		if (Interactable)
		{
			InGameUI->SetCanInteract(bCanInteract, Interactable->GetInteractText());
		}
		else
		{
			InGameUI->SetCanInteract(bCanInteract, FText::FromString(TEXT("")));
		}
	});
}

void ABRPlayerController::SetupInputComponent()
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
	
	UE_LOG(LogTemp, Error, TEXT("ABRPlayerController: The input component is null."))
}

void ABRPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	OwningCharacter = Cast<ABRPlayerCharacter>(InPawn);

	if (IsLocalPlayerController())
	{
		SetupInputMappingContext();
	}
}

void ABRPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();

	OwningCharacter = Cast<ABRPlayerCharacter>(GetPawn());
	
	SetupInputMappingContext();	
}

void ABRPlayerController::SetupInputMappingContext()
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

			UE_LOG(LogTemp, Error, TEXT("ABRPlayerController: The mapping context cannot be null."))
		}
	}
}

void ABRPlayerController::OnMove(const FInputActionValue& InputActionValue) const
{
	OwningCharacter->OnMove(InputActionValue);
}

void ABRPlayerController::OnLook(const FInputActionValue& InputActionValue)
{
	const FVector2D LookInput{InputActionValue.Get<FVector2D>()};
	
	AddPitchInput(LookInput.Y);
	AddYawInput(LookInput.X);
}

void ABRPlayerController::OnJump(const FInputActionValue& InputActionValue) const
{
	OwningCharacter->OnJump(InputActionValue);
}

void ABRPlayerController::OnInteract(const FInputActionValue& InputActionValue) const
{
	OwningCharacter->OnTryInteract();
}

void ABRPlayerController::OnCrouch(const FInputActionValue& InputActionValue) const
{
	if (InputActionValue.Get<bool>())
	{ 
		OwningCharacter->Crouch();
	}
	else
	{ 
		OwningCharacter->UnCrouch();
	}
}

void ABRPlayerController::OnSprint(const FInputActionValue& InputActionValue) const
{
	OwningCharacter->OnSprint(InputActionValue);
}


// void ABRPlayerController::OnCreateSession_Debug()
// {
// 	UBROnlineSubsystem::Get(GetWorld())->CreateSession();
// }

// void ABRPlayerController::OnExternalUIChange(bool bIsOpening)
// {
// 	FlushPressedKeys();
// 	
// 	if (bIsOpening)
// 	{
// 		bShowMouseCursor = true;
// 		bEnableClickEvents = true;
// 		bEnableMouseOverEvents = true;
//
// 		FInputModeUIOnly InputMode;
// 		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
// 		SetInputMode(InputMode);
// 	}
// 	else
// 	{
// 		bShowMouseCursor = false;
// 		bEnableClickEvents = false;
// 		bEnableMouseOverEvents = false;
//
// 		FInputModeGameOnly InputMode;
// 		SetInputMode(InputMode);
// 	}
// }
