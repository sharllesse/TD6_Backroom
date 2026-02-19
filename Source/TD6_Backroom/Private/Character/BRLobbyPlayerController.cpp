// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BRLobbyPlayerController.h"

#include "Chain.h"
#include "GameMode/BRLobbyGameMode.h"
#include "PlayerState/BRLobbyPlayerState.h"
#include "UI/UIManagerSubsystem.h"
#include "UI/LobbyInfoWidget.h"

void ABRLobbyPlayerController::Server_SetReady_Implementation(bool bIsReady)
{
	Chain::Execute(GetPlayerState<ABRLobbyPlayerState>(), [&](ABRLobbyPlayerState* LobbyPlayerState)
	{
		LobbyPlayerState->SetIsReady(bIsReady);
		
		Chain::Execute(GetWorld()->GetAuthGameMode<ABRLobbyGameMode>(), [this](ABRLobbyGameMode* GameMode)
		{
			GameMode->CheckIfAllPlayersReady();
		});
	});
}

void ABRLobbyPlayerController::ToggleReadyState()
{
	Chain::Execute(GetPlayerState<ABRLobbyPlayerState>(), [&](ABRLobbyPlayerState* LobbyPlayerState)
	{
		Server_SetReady(!LobbyPlayerState->GetIsReady());
	});
}

void ABRLobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	auto OptionalLobbyInfo = Chain::StartChain(GetLocalPlayer())
	.Transform([](ULocalPlayer* LocalPlayer)
	{
		return LocalPlayer->GetSubsystem<UUIManagerSubsystem>();
	})
	.GetValue([](UUIManagerSubsystem* UIManager)
	{
		return UIManager->PushMenu<ULobbyInfoWidget>();
	});
	
	if (OptionalLobbyInfo)
	{
		LobbyInfoWidget = *OptionalLobbyInfo;
	}
	else
	{
		return;
	}
	
	LobbyInfoWidget->SetOnSetReadyCallback([this]
	{
		ToggleReadyState();
	});
}
