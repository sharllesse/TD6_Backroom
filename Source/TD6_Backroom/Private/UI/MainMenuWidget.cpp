// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenuWidget.h"

#include "OnlineSubsystemUtils.h"
#include "Components/Button.h"
#include "Interfaces/OnlinePresenceInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Online/BROnlineSubsystem.h"
#include "UI/CreateRoomWidget.h"
#include "UI/OptionsWidget.h"
#include "UI/SearchLobbyWidget.h"
#include "UI/UIManagerSubsystem.h"

void UMainMenuWidget::OnCreateRoomClicked()
{
	Chain::StartChain(GetOwningLocalPlayer())
	.Transform([](const ULocalPlayer* LocalPlayer)
	{
		return LocalPlayer->GetSubsystem<UUIManagerSubsystem>();
	})
	.Execute([](UUIManagerSubsystem* UIManager)
	{
		UIManager->PushMenu<UCreateRoomWidget>();
	});	
}

void UMainMenuWidget::OnJoinRoomClicked()
{
	Chain::StartChain(GetOwningLocalPlayer())
	.Transform([](const ULocalPlayer* LocalPlayer)
	{
		return LocalPlayer->GetSubsystem<UUIManagerSubsystem>();
	})
	.Execute([](UUIManagerSubsystem* UIManager)
	{
		UIManager->PushMenu<USearchLobbyWidget>();
	});	
}

void UMainMenuWidget::OnOptionsClicked()
{
	Chain::StartChain(GetOwningLocalPlayer())
	.Transform([](const ULocalPlayer* LocalPlayer)
	{
		return LocalPlayer->GetSubsystem<UUIManagerSubsystem>();
	})
	.Execute([](UUIManagerSubsystem* UIManager)
	{
		UIManager->PushMenu<UOptionsWidget>();
	});	
}

void UMainMenuWidget::OnQuitClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningLocalPlayer()->GetPlayerController(GetWorld()), EQuitPreference::Quit, true);
}

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CreateRoomButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnCreateRoomClicked);
	JoinRoomButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnJoinRoomClicked);
	OptionsButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnOptionsClicked);
	QuitButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnQuitClicked);


}

void UMainMenuWidget::OnLogin()
{
	Chain::Execute(UBROnlineSubsystem::Get(GetWorld()), [](UBROnlineSubsystem* Subsystem)
	{
		Subsystem->UpdatePresence(EOnlinePresenceState::Online, TEXT("In menu"), false);
	});
}
