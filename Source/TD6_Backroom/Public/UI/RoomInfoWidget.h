// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "Blueprint/UserWidget.h"
#include "RoomInfoWidget.generated.h"

class UTextBlock;
class UButton;
/**
 * 
 */
UCLASS()
class TD6_BACKROOM_API URoomInfoWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> JoinButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> RoomOwner;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> RoomName;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayerCount;

	UFUNCTION()
	void OnJoinButtonClicked();

	FOnlineSessionSearchResult SessionInfo;
public:
	virtual void NativeConstruct() override;

	void SetupRoomInfo(const FOnlineSessionSearchResult& SessionSearchResult);
};
