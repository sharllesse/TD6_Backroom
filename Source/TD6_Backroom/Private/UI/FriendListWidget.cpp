// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FriendListWidget.h"

#include "EventBus.h"
#include "Components/ScrollBox.h"
#include "Interfaces/OnlinePresenceInterface.h"
#include "Online/BROnlineGameTags.h"
#include "Interfaces/OnlinePresenceInterface.h"
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

	UE_LOG(LogTemp, Error, TEXT("%s"), *PresenceInfo.ToDebugString());
	
	
	UpdatePresence(OnlineFriend->GetPresence(), UserInfoWidget);
	UpdateActivity(OnlineFriend, UserInfoWidget);
	
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

void UFriendListWidget::UpdatePresence(const FOnlineUserPresence& PresenceInfo,
	const TStrongObjectPtr<UUserInfoWidget>& UserInfoWidget) const
{
	switch (PresenceInfo.Status.State) {
	case EOnlinePresenceState::Online:
		UserInfoWidget->SetPresence(TEXT("Online"), FColor::Green);
		break;
	case EOnlinePresenceState::Offline:
		UpdateOfflineTimeUser(PresenceInfo, UserInfoWidget);
		break;
	case EOnlinePresenceState::Away:
		UserInfoWidget->SetPresence(TEXT("Away"), FColor::Orange);
		break;
	case EOnlinePresenceState::ExtendedAway:
		UserInfoWidget->SetPresence(TEXT("Away"), FColor::Orange);
		break;
	case EOnlinePresenceState::DoNotDisturb:
		UserInfoWidget->SetPresence(TEXT("Do not disturb"), FColor::Red);
		break;
	case EOnlinePresenceState::Chat:
		break;
	}
}

void UFriendListWidget::UpdateActivity(const TSharedPtr<FOnlineFriend>& OnlineFriend,
	const TStrongObjectPtr<UUserInfoWidget>& UserInfoWidget) const
{
	
}

void UFriendListWidget::UpdateLocalPlayer() const
{

	IOnlineSubsystem* OnlineSub = Online::GetSubsystem(GetWorld());
	if (!OnlineSub) return;

	IOnlineIdentityPtr IdentityInterface = OnlineSub->GetIdentityInterface();
	IOnlinePresencePtr PresenceInterface = OnlineSub->GetPresenceInterface();
	if (!IdentityInterface.IsValid() || !PresenceInterface.IsValid()) return;
    
	TSharedPtr<const FUniqueNetId> UserId = IdentityInterface->GetUniquePlayerId(0);
	if (!UserId.IsValid()) return;

	PresenceInterface->QueryPresence(*UserId, IOnlinePresence::FOnPresenceTaskCompleteDelegate::CreateUObject(
		this, &UFriendListWidget::OnQueryLocalPresenceComplete));

	
	LocalPlayer->SetCanBeInvited(false);
	LocalPlayer->SetCanBeJoin(false);
	LocalPlayer->SetUserName(IdentityInterface->GetPlayerNickname(0));
	LocalPlayer->SetActivity("");
}

void UFriendListWidget::OnQueryLocalPresenceComplete(const FUniqueNetId& UserId, const bool bWasSuccessful) const
{
	if (!bWasSuccessful) return;

	IOnlineSubsystem* OnlineSub = Online::GetSubsystem(GetWorld());
	if (!OnlineSub) return;

	IOnlinePresencePtr PresenceInterface = OnlineSub->GetPresenceInterface();
	IOnlineIdentityPtr IdentityInterface = OnlineSub->GetIdentityInterface();
	if (!PresenceInterface.IsValid() || !IdentityInterface.IsValid()) return;

	TSharedPtr<FOnlineUserPresence> OutPresence;
	if (PresenceInterface->GetCachedPresence(UserId, OutPresence) == EOnlineCachedResult::Success)
	{
		UpdatePresence(*OutPresence, TStrongObjectPtr<UUserInfoWidget>(LocalPlayer));
	}
}

void UFriendListWidget::NativeDestruct()
{
	Super::NativeDestruct();

	FriendsMap.Empty();
}


void UFriendListWidget::UpdateUser(const TArray<TSharedRef<FOnlineFriend>>& FriendList)
{
	UUIManagerSubsystem* UIManagerSubsystem = GetOwningLocalPlayer()->GetSubsystem<UUIManagerSubsystem>();
	if (!UIManagerSubsystem)
	{
		return;
	}

	for (auto Friend : FriendsMap)
	{
		Friend.Value->RemoveFromParent();
	}
	FriendsMap.Empty();
	
	for (auto Friend : FriendList)
	{
			UUserInfoWidget* UserInfoWidget = UIManagerSubsystem->CreateWidget<UUserInfoWidget>();
			FriendListBox->AddChild(UserInfoWidget);
			FriendsMap.Add(Friend.ToSharedPtr(), TStrongObjectPtr(UserInfoWidget));
	}

	for (auto Friend : FriendsMap)
	{
		UpdateUser(Friend.Key, Friend.Value);
	}
	
}

void UFriendListWidget::UpdateUser(const FUniqueNetId& UserId, const TSharedRef<FOnlineUserPresence>& Presence)
{
	if(auto Friend = Online::GetFriendsInterface(GetWorld())->GetFriend(0,UserId, EFriendsLists::ToString(EFriendsLists::Default)))
	{
		if (FriendsMap.Contains(Friend))
		{
			UpdateUser(Friend, FriendsMap[Friend]);
		}
	}
}
