// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RoomInfoWidget.h"

#include "Chain.h"
#include "EventBus.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Online/BROnlineGameTags.h"
#include "Online/BROnlineSubsystem.h"

void URoomInfoWidget::OnJoinButtonClicked()
{
	Chain::Execute(UBROnlineSubsystem::Get(GetWorld()), [&](UBROnlineSubsystem* Object)
	{
		Object->JoinSession(SessionInfo);
	});

	
}

void URoomInfoWidget::NativeDestruct()
{
	Super::NativeDestruct();
	UEventBus::Remove(this,Online_Callback_OnJoinSessionCompleted , OnJoinCompletedHandle);
}

void URoomInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	JoinButton->OnClicked.AddDynamic(this, &ThisClass::OnJoinButtonClicked);
	UEventBus::AddLambda(this, Online_Callback_OnJoinSessionCompleted, [&](EOnJoinSessionCompleteResult::Type Result)
	{
		Chain::Execute(UBROnlineSubsystem::Get(GetWorld()), [](UBROnlineSubsystem* Subsystem)
		{
			Subsystem->UpdatePresence(EOnlinePresenceState::Online, TEXT("Waiting in lobby"), true);
		});
	});
}

void URoomInfoWidget::SetupRoomInfo(const FOnlineSessionSearchResult& SessionSearchResult)
{
	FString SessionName;
	SessionSearchResult.Session.SessionSettings.Get(Online_Settings_Session_Name, SessionName);
	RoomName->SetText(FText::FromString(SessionName));
	RoomOwner->SetText(FText::FromString(SessionSearchResult.Session.OwningUserName));
	int MaxPlayer = SessionSearchResult.Session.SessionSettings.NumPublicConnections;
	int CurrentPlayer = FMath::Abs(SessionSearchResult.Session.NumOpenPublicConnections - MaxPlayer);
	PlayerCount->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), CurrentPlayer, MaxPlayer)));
	SessionInfo = SessionSearchResult;
}
