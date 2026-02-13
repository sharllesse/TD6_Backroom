// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FriendListWidget.h"

#include "EventBus.h"
#include "Components/ScrollBox.h"
#include "Interfaces/OnlinePresenceInterface.h"
#include "Online/BROnlineGameTags.h"
#include "Online/BROnlineSubsystem.h"
#include "UI/UIManagerSubsystem.h"
#include "UI/UserInfoWidget.h"

 
#include "OnlineSubsystemUtils.h" 

void UFriendListWidget::UpdateUser(const TSharedPtr<FOnlineFriend>& OnlineFriend, const TStrongObjectPtr<UUserInfoWidget>& UserInfoWidget) const
{
	auto OnlineSubsystem = GetGameInstance()->GetSubsystem<UBROnlineSubsystem>();
	if (!OnlineFriend || !UserInfoWidget || !OnlineSubsystem)
	{
		return;
	}
	
	const auto& PresenceInfo = OnlineFriend->GetPresence();
	UserInfoWidget->SetActivity(PresenceInfo.Status.StatusStr);
	UserInfoWidget->SetUserName(OnlineFriend->GetDisplayName());
	
	if (!PresenceInfo.bIsOnline)
	{
		UpdateOfflineTimeUser(PresenceInfo, UserInfoWidget);
	}
	else if (PresenceInfo.bIsOnline)
	{
		if (PresenceInfo.bIsPlayingThisGame || !PresenceInfo.Status.StatusStr.IsEmpty())
		{
			FString Status = FString::Printf(TEXT("Playing %s"), *PresenceInfo.Status.StatusStr);
			UserInfoWidget->SetPresence(Status, FColor::Green);
		}
		else
		{
			UserInfoWidget->SetPresence(TEXT("Online"), FColor::Blue);
		}
	}
	
	UserInfoWidget->SetCanBeJoin(PresenceInfo.bIsJoinable);
	UserInfoWidget->SetCanBeInvited(PresenceInfo.bIsPlayingThisGame);
}

void UFriendListWidget::UpdateOfflineTimeUser(const FOnlineUserPresence& OnlineFriendPresence,
	const TStrongObjectPtr<UUserInfoWidget>& UserInfoWidget) const
{
	FString PresenceStatus;
	if (!OnlineFriendPresence.bIsOnline)
	{
		const FDateTime Now = FDateTime::Now();
		const FTimespan TimeDiff = Now - OnlineFriendPresence.LastOnline;

		if (TimeDiff.GetTotalDays() >= 1)
		{
			int32 Days = static_cast<int32>(TimeDiff.GetTotalDays());
			FString Suffix = (Days > 1) ? TEXT("s") : TEXT("");
			PresenceStatus = FString::Printf(TEXT("Offline for %d Day%s"), Days, *Suffix);
		}
		else if (TimeDiff.GetTotalHours() >= 1)
		{
			int32 Hours = static_cast<int32>(TimeDiff.GetTotalHours());
			PresenceStatus = FString::Printf(TEXT("Offline for %dh"), Hours);
		}
		else
		{
			int32 Minutes = static_cast<int32>(TimeDiff.GetTotalMinutes());
			if (Minutes <= 0)
			{
				PresenceStatus = TEXT("Offline just now");
			}
			else
			{
				PresenceStatus = FString::Printf(TEXT("Offline for %dm"), Minutes);
			}
		}
	}
	
	UserInfoWidget->SetPresence(PresenceStatus, FColor::Red);
}

void UFriendListWidget::UpdateLocalPlayer() const
{	
	LocalPlayer->SetCanBeInvited(false);
	LocalPlayer->SetCanBeJoin(false);
	LocalPlayer->SetUserName(Online::GetIdentityInterface(GetWorld())->GetPlayerNickname(0));
}

void UFriendListWidget::UpdateUser(const TArray<TSharedRef<FOnlineFriend>>& FriendList)
{
	UUIManagerSubsystem* UIManagerSubsystem = GetOwningLocalPlayer()->GetSubsystem<UUIManagerSubsystem>();
	if (!UIManagerSubsystem)
	{
		return;
	}
	
	for (auto Friend : FriendList)
	{
		if (!FriendsMap.Contains(Friend.ToWeakPtr()))
		{
			UUserInfoWidget* UserInfoWidget = UIManagerSubsystem->CreateWidget<UUserInfoWidget>();
			FriendListBox->AddChild(UserInfoWidget);
			FriendsMap.Add(Friend.ToWeakPtr(),UserInfoWidget);
		}
	}

	for (auto Friend : FriendsMap)
	{
		UpdateUser(Friend.Key.Pin(), Friend.Value.Pin());
	}
	
}
