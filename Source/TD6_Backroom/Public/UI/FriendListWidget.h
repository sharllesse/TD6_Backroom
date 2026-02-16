// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemTypes.h"
#include "Blueprint/UserWidget.h"
#include "FriendListWidget.generated.h"

class UScrollBox;
class UUserInfoWidget;
/**
 * 
 */
UCLASS()
class TD6_BACKROOM_API UFriendListWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUserInfoWidget> LocalPlayer;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> FriendListBox;
	 
	TMap<TWeakPtr<FOnlineFriend>, TWeakObjectPtr<UUserInfoWidget>> FriendsMap;

	void UpdateUser(const TSharedPtr<FOnlineFriend>& OnlineFriend, const TStrongObjectPtr<UUserInfoWidget>& UserInfoWidget) const;
	void UpdateOfflineTimeUser(const FOnlineUserPresence& OnlineFriendPresence, const TStrongObjectPtr<UUserInfoWidget>& UserInfoWidget) const;
	void UpdatePresence(const FOnlineUserPresence& PresenceInfo, const TStrongObjectPtr<UUserInfoWidget>& UserInfoWidget) const;
	void UpdateActivity(const TSharedPtr<FOnlineFriend>& OnlineFriend, const TStrongObjectPtr<UUserInfoWidget>& UserInfoWidget) const;
	void OnQueryLocalPresenceComplete(const FUniqueNetId& UserId, const bool bWasSuccessful) const;
public:
	
	
	void UpdateLocalPlayer() const;
	void UpdateUser(const TArray<TSharedRef<FOnlineFriend>>&);
	void UpdateUser(const FUniqueNetId& UserId, const TSharedRef<FOnlineUserPresence>& Presence);
	
};
