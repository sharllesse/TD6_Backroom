// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/BRPlayerStateGameTags.h"

UE_DEFINE_GAMEPLAY_TAG_COMMENT(PlayerState_Callback_LobbyReadyChange, "PlayerState.Callback.LobbyReadyChange", 
	"A callback that trigger in lobby when a player state change is bIsReady value"
	"[Args: ABRLobbyPlayerState* PlayerState, bool bIsReady]")

UE_DEFINE_GAMEPLAY_TAG_COMMENT(PlayerState_Callback_LeaveLobby, "PlayerState.Callback.LeaveLobby", 
	"A callback that trigger when a player leave the lobby (and the session)"
	"[Args: ABRLobbyPlayerState* PlayerState]")

UE_DEFINE_GAMEPLAY_TAG_COMMENT(PlayerState_Callback_IsAllPlayerReady, "PlayerState.Callback.IsAllPlayerReady",
	"A callback that trigger when all the player are ready or not"
	"[Args: bool bIsAllPlayerReady]")


UE_DEFINE_GAMEPLAY_TAG_COMMENT(PlayerState_Callback_Dies, "PlayerState.Callback.Dies",
	"A callback that trigger when a player dies or disconnect."
	"[Args: ]")