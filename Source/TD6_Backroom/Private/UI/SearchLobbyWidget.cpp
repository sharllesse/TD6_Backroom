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
	
	RoomsInfo.Empty();
	RoomScrollBox->ClearChildren();	

	for (const auto& Session : SessionsResult)
	{
		if (Session.Session.SessionSettings.NumPrivateConnections > 0)
			continue;

		SetupNewSession(Session);		
	}
}

void USearchLobbyWidget::SetupNewSession(const FOnlineSessionSearchResult& SessionResult)
{
	auto RoomInfo = Chain::StartChain(GetOwningLocalPlayer())
		.Transform([](const ULocalPlayer* LocalPlayer)
		{
			return LocalPlayer->GetSubsystem<UUIManagerSubsystem>();
		})
		.Transform([](UUIManagerSubsystem* UIManager)
		{
			return UIManager->CreateWidget<URoomInfoWidget>(false);
		});

	auto SizeBox = WidgetTree->ConstructWidget<USizeBox>();
	SizeBox->SetMinDesiredHeight(100.f);
	SizeBox->AddChild(RoomInfo);
	
	RoomsInfo.Add(TStrongObjectPtr(RoomInfo.Get()));
	
	
	auto ScrollBoxSlot = Cast<UScrollBoxSlot>(RoomScrollBox->AddChild(SizeBox));
	ScrollBoxSlot->SetHorizontalAlignment(HAlign_Fill);
	ScrollBoxSlot->SetVerticalAlignment(VAlign_Fill);
	ScrollBoxSlot->SetSize(ESlateSizeRule::Automatic);

		
	RoomInfo->SetupRoomInfo(SessionResult);
}

void USearchLobbyWidget::NativeDestruct()
{
	Super::NativeDestruct();
	RoomsInfo.Empty();
	Chain::Execute(UBROnlineSubsystem::Get(GetWorld()), [](UBROnlineSubsystem* Subsystem)
	{
		Subsystem->StopRefreshSessionTimer();
	});
	UEventBus::Remove(this, Online_Callback_OnFindSessionsCompleted, FindSessionDelegateHandle);
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
		Subsystem->LaunchRefreshSessionsTimer(10.f);
	});
}
