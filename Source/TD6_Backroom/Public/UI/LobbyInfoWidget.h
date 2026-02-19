// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EventBus.h"
#include "GameplayTagContainer.h"
#include "OnlineSessionSettings.h"
#include "TimerHolder.h"
#include "Blueprint/UserWidget.h"
#include "Components/VerticalBox.h"
#include "LobbyInfoWidget.generated.h"

class ABRLobbyPlayerState;
class UUserInfoWidget;
class UTextBlock;
class UButton;
/**
 * 
 */
UCLASS()
class TD6_BACKROOM_API ULobbyInfoWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> UserInfoBox;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> LeaveLobby;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SetReady;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> StartingTimer;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayerCount;
	
	
	TFunction<void()> OnSetReadyCallback;
	
	UPROPERTY()
	TMap<ABRLobbyPlayerState*, UUserInfoWidget*> PlayerInfoMap;
	
	UFUNCTION()
	void OnSetReadyClicked();
	UFUNCTION()
	void OnLeaveLobbyClicked();

	void UpdatePlayerCount();

	template<typename Func>
	void AddEventBusDelegate(const FGameplayTag& Tag, Func&& Function)
	{
		auto Delegate = UEventBus::AddLambda(this, Tag,Forward<Func>(Function));
		DelegateMap.Add(Tag, Delegate);
	}
	
	void CreateNewPlayerInfo(ABRLobbyPlayerState* PlayerState);
	void UpdatePlayerInfo(ABRLobbyPlayerState* PlayerState);
	void RemovePlayerInfo(ABRLobbyPlayerState* PlayerState);

	UFUNCTION()
	void UpdateStartingTimer();
	
	virtual void NativeDestruct() override;
	virtual void NativeConstruct() override;

	FTimerHolder UpdateTimerHolder;
	TMap<FGameplayTag, FDelegateHandle> DelegateMap;
	
	int StartingTimerValue{5};
public:
	
	template<typename Func>
	void SetOnSetReadyCallback(Func&& Function)
	{
		OnSetReadyCallback = Forward<Func>(Function);
	} 
};
