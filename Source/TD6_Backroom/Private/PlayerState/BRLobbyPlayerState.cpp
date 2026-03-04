// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/BRLobbyPlayerState.h"

#include "EventBus.h"
#include "Net/UnrealNetwork.h"
#include "PlayerState/BRPlayerStateGameTags.h"


void ABRLobbyPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ABRLobbyPlayerState, bIsReady);
}

void ABRLobbyPlayerState::OnRep_IsReady()
{ 
	UEventBus::Broadcast(this, PlayerState_Callback_LobbyReadyChange, this , bIsReady);
}

void ABRLobbyPlayerState::BeginPlay()
{
	Super::BeginPlay();
	UEventBus::LockSignature<ABRLobbyPlayerState*, bool>(this, PlayerState_Callback_LobbyReadyChange);
	UEventBus::LockSignature<ABRLobbyPlayerState*>(this, PlayerState_Callback_LeaveLobby);
	UEventBus::Broadcast(this, PlayerState_Callback_LobbyReadyChange, this , bIsReady);
	UEventBus::Broadcast(this, PlayerState_Callback_OnPlayerJoin, this);
}

void ABRLobbyPlayerState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UEventBus::Broadcast(this, PlayerState_Callback_LeaveLobby, this);
	UEventBus::UnlockSignature(this, PlayerState_Callback_LobbyReadyChange);
	UEventBus::UnlockSignature(this, PlayerState_Callback_LeaveLobby);
}

void ABRLobbyPlayerState::SetIsReady(bool bNewReady)
{
	bIsReady = bNewReady;
	
	if (HasAuthority())
	{
		OnRep_IsReady();
	}
}

bool ABRLobbyPlayerState::GetIsReady() const
{
	return bIsReady;
}
