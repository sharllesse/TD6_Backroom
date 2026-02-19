// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/BRLobbyGameMode.h"

#include "Chain.h"
#include "EventBus.h"
#include "Linq.h"
#include "GameFramework/GameStateBase.h"
#include "GameState/BRLobbyGameState.h"
#include "Online/BROnlineSubsystem.h"
#include "PlayerState/BRLobbyPlayerState.h"
#include "PlayerState/BRPlayerStateGameTags.h"

ABRLobbyGameMode::ABRLobbyGameMode()
{
	bUseSeamlessTravel = true;
}

void ABRLobbyGameMode::CheckIfAllPlayersReady() const
{
	Chain::Execute(GetGameState<AGameStateBase>(), [this](AGameStateBase* GameStateBase)
	{
		bool bIsAllReady = Linq::Start(GameStateBase->PlayerArray)
		.Select([](TObjectPtr<APlayerState> PlayerState)
		{
			return PlayerState.Get();
		})
		.Cast<ABRLobbyPlayerState>()
		.All([](ABRLobbyPlayerState* PlayerState)
		{
			return PlayerState->GetIsReady();
		});

		
		Chain::Execute(GetGameState<ABRLobbyGameState>(), [bIsAllReady](ABRLobbyGameState* LobbyGameState)
		{
			LobbyGameState->Multicast_OnAllPlayerReady(bIsAllReady);
		});
	});
}

void ABRLobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
}
