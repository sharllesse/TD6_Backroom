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
#include "UI/CreateRoomWidget.h"
#include "UI/UIManagerSubsystem.h"

void ABRPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	MainMenu = Chain::StartChain(GetLocalPlayer())
	.Transform([](ULocalPlayer* LocalPlayer)
	{
		return LocalPlayer->GetSubsystem<UUIManagerSubsystem>();
	})
	.Transform([](UUIManagerSubsystem* UIManager)
	{
		return UIManager->PushMenu<UMainMenuWidget>();
	});

	auto Handle = UEventBus::AddLambda(this, Online_Callback_OnLoginComplete, [&]( int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
	{
		Chain::StartChain(GetGameInstance())
		.Transform([](const UGameInstance* GameInstance)
		{
			return GameInstance->GetSubsystem<UBROnlineSubsystem>();
		})
		.Execute(&UBROnlineSubsystem::QueryFriendList);
		
		Chain::StartChain(MainMenu.Get())
		.Transform([](UMainMenuWidget* Widget)
		{
			return Widget->FriendList.Get();
		})
		.Execute(&UFriendListWidget::UpdateLocalPlayer);

		SetInputMode(FInputModeUIOnly());
		bShowMouseCursor = true;
	});
	DelegateHandles.Add(Online_Callback_OnLoginComplete, Handle);
	
	Handle = UEventBus::AddLambda(this, Online_Callback_OnReadFriendsListCompleted,
		[&](int32 LocalUserNum, bool bWasSuccessful, const TArray<TSharedRef<FOnlineFriend>>& OnlineFriends, const FString& ErrorStr)
	{
		if (bWasSuccessful)
		{
			Chain::StartChain(MainMenu.Get())
			.Transform([](UMainMenuWidget* Widget)
			{
				return Widget->FriendList.Get();
			})
			.Execute([&](UFriendListWidget* FriendListWidget)
			{
				FriendListWidget->UpdateUser(OnlineFriends);
			});
		}
	});
	DelegateHandles.Add(Online_Callback_OnReadFriendsListCompleted, Handle);
	
	Handle = UEventBus::AddLambda(this, Online_Callback_OnPresenceReceived,
		[&](const FUniqueNetId& UserId, const TSharedRef<FOnlineUserPresence>& Presence)
	{
		Chain::StartChain(MainMenu.Get())
		.Transform([](UMainMenuWidget* Widget)
		{
			return Widget->FriendList.Get();
		})
		.Execute([&](UFriendListWidget* FriendListWidget)
		{
			FriendListWidget->UpdateUser(UserId, Presence);
		});
	});

	DelegateHandles.Add(Online_Callback_OnPresenceReceived, Handle);
	

	if (Online::GetIdentityInterface(GetWorld())->GetLoginStatus(0) == ELoginStatus::LoggedIn)
	{
		Chain::StartChain(GetGameInstance())
		.Transform([](const UGameInstance* GameInstance)
		{
			return GameInstance->GetSubsystem<UBROnlineSubsystem>();
		})
		.Execute(&UBROnlineSubsystem::QueryFriendList);

		Chain::StartChain(MainMenu.Get())
		.Transform([](UMainMenuWidget* Widget)
		{
			return Widget->FriendList.Get();
		})
		.Execute(&UFriendListWidget::UpdateLocalPlayer);
	}
	
	
	//UEventBus::AddUObject(this, Online_Callback_OnExternalUIChange, this, &ABRPlayerController::OnExternalUIChange);
}

void ABRPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	for (const auto& Element : DelegateHandles)
	{
		UEventBus::Remove(this, Element.Key, Element.Value);
	}
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
