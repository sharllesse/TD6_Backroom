// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SearchLobbyWidget.generated.h"

class URoomInfoWidget;
class USizeBox; 
class UScrollBox;
class UEditableTextBox;
class UButton;
/**
 * 
 */
UCLASS()
class TD6_BACKROOM_API USearchLobbyWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> SearchBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> RoomScrollBox;

	TArray<TStrongObjectPtr<URoomInfoWidget>> RoomsInfo;
	
	UFUNCTION()
	void PopThisMenu();

	FDelegateHandle FindSessionDelegateHandle;
	
	void OnSessionsFind(const TArray<FOnlineSessionSearchResult>& SessionsResult, bool bWasSuccessful);

	void SetupNewSession(const FOnlineSessionSearchResult& SessionResult);

	virtual void NativeDestruct() override;
	
public:
	virtual void NativeConstruct() override;

	
};
