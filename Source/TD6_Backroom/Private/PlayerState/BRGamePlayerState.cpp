// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/BRGamePlayerState.h"

#include "EventBus.h"
#include "PlayerState/BRPlayerStateGameTags.h"

void ABRGamePlayerState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	NotifiesAPlayerDied_Implementation();
}

void ABRGamePlayerState::NotifiesAPlayerDied_Implementation()
{
	UEventBus::Broadcast(this, PlayerState_Callback_Dies);
}

void ABRGamePlayerState::SetToSpectator()
{
	SetIsSpectator(true);
	
	NotifiesAPlayerDied_Implementation();
}
