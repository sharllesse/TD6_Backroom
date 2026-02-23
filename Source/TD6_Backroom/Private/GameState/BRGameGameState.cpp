// Fill out your copyright notice in the Description page of Project Settings.


#include "GameState/BRGameGameState.h"

#include "Net/UnrealNetwork.h"

void ABRGameGameState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABRGameGameState, ItemMap);
}
