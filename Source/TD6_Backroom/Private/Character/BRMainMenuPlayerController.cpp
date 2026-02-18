// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BRMainMenuPlayerController.h"

#include "Chain.h"
#include "EventBus.h"
#include "OnlineSubsystemUtils.h"
#include "Online/BROnlineGameTags.h"
#include "Online/BROnlineSubsystem.h"
#include "UI/UIManagerSubsystem.h"
#include "UI/MainMenuWidget.h"

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
		
		Chain::Execute(MainMenu.Get(),[](UMainMenuWidget* Widget)
		{
			Widget->OnLogin();
		});
	});
	DelegateHandles.Add(Online_Callback_OnLoginComplete, Handle);
	

	if (Online::GetIdentityInterface(GetWorld())->GetLoginStatus(0) == ELoginStatus::LoggedIn)
	{
		Chain::Execute(MainMenu.Get(),[](UMainMenuWidget* Widget)
		{
			Widget->OnLogin();
		});
	}
	
	UEventBus::AddLambda(this, Online_Callback_OnMainExternalUIOverlayChange, [](bool bIsOpening)
	{
		UE_LOG(LogTemp, Error, TEXT("Epic main overlay is opening ? = %d"), static_cast<int>(bIsOpening));
	});
	
}

void ABRMainMenuPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	for (const auto& Element : DelegateHandles)
	{
		UEventBus::Remove(this, Element.Key, Element.Value);
	}
}
