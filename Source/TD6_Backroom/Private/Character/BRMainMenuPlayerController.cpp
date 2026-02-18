// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BRMainMenuPlayerController.h"

#include "Chain.h"
#include "EventBus.h"
#include "OnlineSubsystemUtils.h"
#include "Online/BROnlineGameTags.h"
#include "Online/BROnlineSubsystem.h"
#include "UI/UIManagerSubsystem.h"
#include "UI/MainMenuWidget.h"
#include "UI/FriendListWidget.h"

void ABRMainMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	SetInputMode(FInputModeUIOnly());
	bShowMouseCursor = true;

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
			Widget->OnLogin();
			return Widget->FriendList.Get();
		})
		.Execute(&UFriendListWidget::UpdateLocalPlayer);
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
			Widget->OnLogin();
			return Widget->FriendList.Get();
		})
		.Execute(&UFriendListWidget::UpdateLocalPlayer);
	}
	
}

void ABRMainMenuPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	for (const auto& Element : DelegateHandles)
	{
		UEventBus::Remove(this, Element.Key, Element.Value);
	}
}
