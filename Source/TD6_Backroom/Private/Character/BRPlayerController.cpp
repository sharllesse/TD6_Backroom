// Copyright (c) Logicraft Interactive. All Rights Reserved.

#include "Character/BRPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "EventBus.h"
#include "OnlineSubsystemUtils.h"
#include "Blueprint/UserWidget.h"
#include "Online/BROnlineGameTags.h"
#include "Online/BROnlineSubsystem.h"
#include "UI/FriendListWidget.h"
#include "UI/MainMenuWidget.h"
#include "UI/UIManagerSubsystem.h"

void ABRPlayerController::BeginPlay()
{
	Super::BeginPlay();

	MainMenu = GetLocalPlayer()->GetSubsystem<UUIManagerSubsystem>()->PushMenu<UMainMenuWidget>();

	UEventBus::AddLambda(this, Online_Callback_OnLoginComplete, [&]( int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
	{
		auto OnlineSubsystem = GetGameInstance()->GetSubsystem<UBROnlineSubsystem>();
		OnlineSubsystem->QueryFriendList();
		Chain::Execute(MainMenu.Get(), [](UMainMenuWidget* Widget)
		{
			Widget->FriendList->UpdateLocalPlayer();
		});
	});
	
	UEventBus::AddLambda(this, Online_Callback_OnReadFriendsListCompleted,
		[&](int32 LocalUserNum, bool bWasSuccessful, const TArray<TSharedRef<FOnlineFriend>>& OnlineFriends, const FString& ErrorStr)
	{
		if (bWasSuccessful)
		{
			MainMenu->FriendList->UpdateUser(OnlineFriends);
		}
	});

	

	
	
	//UEventBus::AddUObject(this, Online_Callback_OnExternalUIChange, this, &ABRPlayerController::OnExternalUIChange);
}

void ABRPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	//UEventBus::RemoveAll(this, Online_Callback_OnExternalUIChange, this);
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

void ABRPlayerController::OnMove(const FInputActionValue& InputActionValue)
{
	OwningCharacter->OnMove(InputActionValue);
}

void ABRPlayerController::OnLook(const FInputActionValue& InputActionValue)
{
	const FVector2D LookInput{InputActionValue.Get<FVector2D>()};
	
	AddPitchInput(LookInput.Y);
	AddYawInput(LookInput.X);
}

void ABRPlayerController::OnJump(const FInputActionValue& InputActionValue)
{
	OwningCharacter->OnJump(InputActionValue);
}

void ABRPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);


	
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
