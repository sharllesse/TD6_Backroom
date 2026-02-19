// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/BRLobbyGameMode.h"

#include "Chain.h"
#include "Linq.h"
#include "GameFramework/GameStateBase.h"
#include "Online/BROnlineSubsystem.h"
#include "PlayerState/BRLobbyPlayerState.h"

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
		
		if (bIsAllReady)
		{
			Chain::Execute(UBROnlineSubsystem::Get(GetWorld()), [this](UBROnlineSubsystem* Subsystem)
			{
				UE_LOG(LogTemp, Error, TEXT("All player are ready!"))
				//Subsystem->StartSession();
			});
		}
	});
}
