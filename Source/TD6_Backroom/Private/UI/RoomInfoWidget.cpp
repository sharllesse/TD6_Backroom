// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RoomInfoWidget.h"

#include "Chain.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Online/BROnlineSubsystem.h"

void URoomInfoWidget::OnJoinButtonClicked()
{
	Chain::Execute(UBROnlineSubsystem::Get(GetWorld()), [&](UBROnlineSubsystem* Object)
	{
		Object->JoinSession(SessionInfo);
	});
}

void URoomInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	JoinButton->OnClicked.AddDynamic(this, &ThisClass::OnJoinButtonClicked);
}

void URoomInfoWidget::SetupRoomInfo(const FOnlineSessionSearchResult& SessionSearchResult)
{
	RoomName->SetText(FText::FromString(SessionSearchResult.GetSessionIdStr()));
	RoomOwner->SetText(FText::FromString(SessionSearchResult.Session.OwningUserName));
	int MaxPlayer = SessionSearchResult.Session.SessionSettings.NumPublicConnections;
	int CurrentPlayer = FMath::Abs(SessionSearchResult.Session.NumOpenPublicConnections - MaxPlayer);
	PlayerCount->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), CurrentPlayer, MaxPlayer)));
	SessionInfo = SessionSearchResult;
}
