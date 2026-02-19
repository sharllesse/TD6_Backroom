// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BRLobbyGameState.generated.h"

/**
 * 
 */
UCLASS()
class TD6_BACKROOM_API ABRLobbyGameState : public AGameStateBase
{
	GENERATED_BODY()
public:

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnAllPlayerReady(bool bIsAllReady);
};
