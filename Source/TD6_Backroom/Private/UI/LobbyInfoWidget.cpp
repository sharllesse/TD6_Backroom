// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LobbyInfoWidget.h"

#include "Chain.h"
#include "EventBus.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBoxSlot.h"
#include "GameFramework/GameStateBase.h"
#include "Online/BROnlineGameTags.h"
#include "Online/BROnlineSubsystem.h"
#include "PlayerState/BRLobbyPlayerState.h"
#include "PlayerState/BRPlayerStateGameTags.h"
#include "UI/UIManagerSubsystem.h"
#include "UI/UserInfoWidget.h"


void ULobbyInfoWidget::NativeDestruct()
{
	Super::NativeDestruct();
	
	PlayerInfoMap.Empty();
	UEventBus::Remove(this, PlayerState_Callback_LobbyReadyChange, OnPlayerStateUpdateDelegate);
}

void ULobbyInfoWidget::OnSetReadyClicked()
{
	if (OnSetReadyCallback.IsSet())
	{
		OnSetReadyCallback();
	}
}

void ULobbyInfoWidget::OnLeaveLobbyClicked()
{
	//TODO Leave Session;
}

void ULobbyInfoWidget::CreateNewPlayerInfo(ABRLobbyPlayerState* PlayerState)
{
	auto PlayerInfo = Chain::StartChain(GetOwningLocalPlayer())
		.Transform([](const ULocalPlayer* LocalPlayer)
		{
			return LocalPlayer->GetSubsystem<UUIManagerSubsystem>();
		})
		.Transform([](UUIManagerSubsystem* UIManager)
		{
			return UIManager->CreateWidget<UUserInfoWidget>(false);
		});
	
	UserInfoBox->AddChildToVerticalBox(PlayerInfo)->SetSize(ESlateSizeRule::Fill);
	PlayerInfo->SetIsReady(PlayerState->GetIsReady());
	PlayerInfo->SetUserName(PlayerState->GetPlayerName());
	
	PlayerInfoMap.Add(PlayerState, PlayerInfo);
}

void ULobbyInfoWidget::UpdatePlayerInfo(ABRLobbyPlayerState* PlayerState)
{
	auto PlayerInfo = PlayerInfoMap[PlayerState];
	PlayerInfo->SetIsReady(PlayerState->GetIsReady());
	PlayerInfo->SetUserName(PlayerState->GetPlayerName());
}

void ULobbyInfoWidget::RemovePlayerInfo(ABRLobbyPlayerState* PlayerState)
{
	PlayerInfoMap[PlayerState]->RemoveFromParent();
	PlayerInfoMap.Remove(PlayerState);
}

void ULobbyInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetReady->OnClicked.AddDynamic(this, &ThisClass::OnSetReadyClicked);
	LeaveLobby->OnClicked.AddDynamic(this, &ThisClass::OnLeaveLobbyClicked);
	
	OnPlayerStateUpdateDelegate = UEventBus::AddLambda(this, PlayerState_Callback_LobbyReadyChange, 
		[&](ABRLobbyPlayerState* PlayerState, bool bIsReady)
	{
			if (!PlayerState)
				return;
			
		if (!PlayerInfoMap.Contains(PlayerState))
		{
			CreateNewPlayerInfo(PlayerState);
		}
		else
		{
			UpdatePlayerInfo(PlayerState);
		}
	});
	
	if (AGameStateBase* GS = GetWorld()->GetGameState())
	{
		for (APlayerState* PS : GS->PlayerArray)
		{
			if (ABRLobbyPlayerState* LobbyPS = Cast<ABRLobbyPlayerState>(PS))
			{
				if (!PlayerInfoMap.Contains(LobbyPS))
				{
					CreateNewPlayerInfo(LobbyPS);
				}
			}
		}
	}
}
