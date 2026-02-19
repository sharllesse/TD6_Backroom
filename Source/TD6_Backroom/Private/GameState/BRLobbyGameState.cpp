// Fill out your copyright notice in the Description page of Project Settings.


#include "GameState/BRLobbyGameState.h"

#include "EventBus.h"
#include "PlayerState/BRPlayerStateGameTags.h"

void ABRLobbyGameState::Multicast_OnAllPlayerReady_Implementation(bool bIsAllReady)
{
	UEventBus::Broadcast(this, PlayerState_Callback_IsAllPlayerReady, bIsAllReady);
}
