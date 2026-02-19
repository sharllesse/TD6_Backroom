// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BRLobbyPlayerController.generated.h"

class ULobbyInfoWidget;
/**
 * 
 */
UCLASS()
class TD6_BACKROOM_API ABRLobbyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	
	UPROPERTY(Transient)
	TObjectPtr<ULobbyInfoWidget> LobbyInfoWidget;
	
	UFUNCTION()
	void ToggleReadyState();
	
	UFUNCTION(Server, Reliable)
	void Server_SetReady(bool bIsReady);
	
	virtual void BeginPlay() override;
};
