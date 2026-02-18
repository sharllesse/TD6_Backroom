// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenuWidget.h"

#include "OnlineSubsystemUtils.h"
#include "Components/Button.h"
#include "Interfaces/OnlinePresenceInterface.h"
#include "UI/CreateRoomWidget.h"
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

void UMainMenuWidget::OnLogin()
{
	auto& LocalId{ *Online::GetIdentityInterface(GetWorld())->GetUniquePlayerId(0) };
	FOnlineUserPresenceStatus NewStatus;
	NewStatus.State = EOnlinePresenceState::Online;
	NewStatus.StatusStr = TEXT("In the menu");
	Online::GetPresenceInterface(GetWorld())->SetPresence(LocalId, NewStatus);
}

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CreateRoomButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnCreateRoomClicked);
	JoinRoomButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnJoinRoomClicked);


}
