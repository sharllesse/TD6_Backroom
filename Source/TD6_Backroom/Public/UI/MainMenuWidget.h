// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UButton;
class UFriendListWidget;
/**
 * 
 */
UCLASS()
class TD6_BACKROOM_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CreateRoomButton;

	UFUNCTION()
	void OnCreateRoomClicked();
public:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UFriendListWidget> FriendList;
	
	virtual void NativeConstruct() override;
};
