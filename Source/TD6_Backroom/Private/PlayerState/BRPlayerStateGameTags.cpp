// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/BRPlayerStateGameTags.h"

UE_DEFINE_GAMEPLAY_TAG_COMMENT(PlayerState_Callback_LobbyReadyChange, "PlayerState.Callback.LobbyReadyChange", 
	"A callback that trigger in lobby when a player state change is bIsReady value"
	"[Args: ABRLobbyPlayerState* PlayerState, bool bIsReady]")