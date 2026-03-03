// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BRMainMenuPlayerController.h"

#include "Chain.h"
#include "EventBus.h"
#include "OnlineSubsystemUtils.h"
#include "GameInstance/Subsystem/VivoxSubsystem.h"
#include "Online/BROnlineGameTags.h"
#include "Online/BROnlineSubsystem.h"
#include "UI/UIManagerSubsystem.h"
#include "UI/MainMenuWidget.h"
#include "UI/WaitingLoginWidget.h"

void ABRMainMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	SetInputMode(FInputModeUIOnly());
	bShowMouseCursor = true;
	
	auto DelegateHandle = UEventBus::AddLambda(this,Online_Callback_OnLoginComplete,
		[this](int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
		{
			if (bWasSuccessful)
			{
				Chain::StartChain(GetGameInstance())
				.Transform([](UGameInstance* GameInstance){return GameInstance->GetSubsystem<UVivoxSubsystem>();})
				.Execute(&UVivoxSubsystem::Login);
				
				Chain::StartChain(GetLocalPlayer())
				.Transform([](ULocalPlayer* LocalPlayer)
				{
					return LocalPlayer->GetSubsystem<UUIManagerSubsystem>();
				})
				.Execute([](UUIManagerSubsystem* UIManager)
				{
					return UIManager->PopMenu();
				});
			}
		});

	DelegateHandles.Add(Online_Callback_OnLoginComplete, DelegateHandle);
	SetupMainMenu();
	
	Chain::Execute(UBROnlineSubsystem::Get(GetWorld()), [this](UBROnlineSubsystem* Subsystem)
	{
		Subsystem->DestroySession();

		if (!Subsystem->IsLogged())
		{
			Chain::StartChain(GetLocalPlayer())
			.Transform([](ULocalPlayer* LocalPlayer)
			{
				return LocalPlayer->GetSubsystem<UUIManagerSubsystem>();
			})
			.Execute([](UUIManagerSubsystem* UIManager)
			{
				return UIManager->PushMenu<UWaitingLoginWidget>();
			});
		}
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

void ABRMainMenuPlayerController::SetupMainMenu()
{
	auto OptionalMainMenu = Chain::StartChain(GetLocalPlayer())
	.Transform([](ULocalPlayer* LocalPlayer)
	{
		return LocalPlayer->GetSubsystem<UUIManagerSubsystem>();
	})
	.GetValue([](UUIManagerSubsystem* UIManager)
	{
		return UIManager->PushMenu<UMainMenuWidget>();
	});
	
	if (OptionalMainMenu)
	{
		MainMenu = *OptionalMainMenu;
	}
	

	if (Online::GetIdentityInterface(GetWorld())->GetLoginStatus(0) == ELoginStatus::LoggedIn)
	{
		Chain::Execute(MainMenu.Get(),[](UMainMenuWidget* Widget)
		{
			Widget->OnLogin();
		});
	}
}
