// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "BRMainMenuPlayerController.generated.h"

class UMainMenuWidget;
class UCreateRoomWidget;
/**
 * 
 */
UCLASS()
class TD6_BACKROOM_API ABRMainMenuPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	UPROPERTY()
	TObjectPtr<UMainMenuWidget> MainMenu;
	
	UPROPERTY()
	TObjectPtr<UCreateRoomWidget> CreateRoomWidget;

	TMap<FGameplayTag, FDelegateHandle> DelegateHandles;
	
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
