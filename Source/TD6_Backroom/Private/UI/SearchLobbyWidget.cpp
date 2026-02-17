// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SearchLobbyWidget.h"

#include "Chain.h"
#include "EventBus.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/SizeBox.h"
#include "Components/PanelSlot.h"
#include "Components/ScrollBoxSlot.h"
#include "Components/SizeBoxSlot.h"
#include "Online/BROnlineGameTags.h"
#include "Online/BROnlineSubsystem.h"
#include "UI/RoomInfoWidget.h"
#include "UI/UIManagerSubsystem.h"
 

void USearchLobbyWidget::PopThisMenu()
{
	Chain::Execute(GetOwningLocalPlayer()->GetSubsystem<UUIManagerSubsystem>(), &UUIManagerSubsystem::PopMenu);
	Chain::Execute(UBROnlineSubsystem::Get(GetWorld()), [](UBROnlineSubsystem* Subsystem)
	{
		Subsystem->StopRefreshSessionTimer();
	});
	UEventBus::Remove(this, Online_Callback_OnFindSessionsCompleted, FindSessionDelegateHandle);
}

void USearchLobbyWidget::OnSessionsFind(const TArray<FOnlineSessionSearchResult>& SessionsResult, bool bWasSuccessful)
{
	if (!bWasSuccessful)
		return;

	RoomsInfo.RemoveAll([](TStrongObjectPtr<URoomInfoWidget> InfoWidget)
	{
		InfoWidget->GetParent()->RemoveFromParent();
		InfoWidget->RemoveFromParent();
		return true;
	});

	for (const auto& Session : SessionsResult)
	{
		if (Session.Session.SessionSettings.NumPrivateConnections > 0)
			continue;

		SetupNewSession(Session);		
	}
}

void USearchLobbyWidget::SetupNewSession(const FOnlineSessionSearchResult& SessionResult)
{
	auto SizeBox = WidgetTree->ConstructWidget<USizeBox>();
	SizeBox->SetMinDesiredHeight(100);
	auto ScrollBoxSlot = Cast<UScrollBoxSlot>(RoomScrollBox->AddChild(SizeBox));
	ScrollBoxSlot->SetHorizontalAlignment(HAlign_Fill);
	ScrollBoxSlot->SetVerticalAlignment(VAlign_Fill);
	ScrollBoxSlot->SetSize(ESlateSizeRule::Automatic);

	auto RoomInfo = Chain::StartChain(GetOwningLocalPlayer())
	.Transform([](const ULocalPlayer* LocalPlayer)
	{
		return LocalPlayer->GetSubsystem<UUIManagerSubsystem>();
	})
	.Transform([](UUIManagerSubsystem* UIManager)
	{
		return UIManager->PushMenu<URoomInfoWidget>();
	});
	
	auto SizeBoxSlot = Cast<USizeBoxSlot>(SizeBox->AddChild(RoomInfo));
	SizeBoxSlot->SetHorizontalAlignment(HAlign_Fill);
	SizeBoxSlot->SetVerticalAlignment(VAlign_Fill);


	RoomInfo->SetupRoomInfo(SessionResult);
	RoomsInfo.Add(TStrongObjectPtr(RoomInfo.Get()));
}


void USearchLobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BackButton->OnClicked.AddDynamic(this, &ThisClass::PopThisMenu);

	FindSessionDelegateHandle = UEventBus::AddLambda(this, Online_Callback_OnFindSessionsCompleted,[&](const TArray<FOnlineSessionSearchResult>& SessionsResult, bool bWasSuccessful)
	{
		OnSessionsFind(SessionsResult, bWasSuccessful);
	});
	
	Chain::StartChain(UBROnlineSubsystem::Get(GetWorld()))
	.Execute([](UBROnlineSubsystem* Subsystem)
	{
		Subsystem->FindSessions();
		Subsystem->LaunchRefreshSessionsTimer(10.f);
	});
}
