// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BRLobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TD6_BACKROOM_API ABRLobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
	 
	
public:
	void CheckIfAllPlayersReady() const;
	
};
