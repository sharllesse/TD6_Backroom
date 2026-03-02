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
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> JoinRoomButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> OptionsButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QuitButton;

	UFUNCTION()
	void OnCreateRoomClicked();
	UFUNCTION()
	void OnJoinRoomClicked();
	UFUNCTION()
	void OnOptionsClicked();
	UFUNCTION()
	void OnQuitClicked();

public:
	
	virtual void NativeConstruct() override;
	
	void OnLogin();
};
