// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LobbyInfoWidget.h"

#include "Chain.h"
#include "EventBus.h"
#include "GameMapsSettings.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBoxSlot.h"
#include "GameFramework/GameStateBase.h"
#include "GameInstance/BRGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Online/BROnlineGameTags.h"
#include "Online/BROnlineSubsystem.h"
#include "PlayerState/BRLobbyPlayerState.h"
#include "PlayerState/BRPlayerStateGameTags.h"
#include "UI/UIManagerSubsystem.h"
#include "UI/UserInfoWidget.h"



void ULobbyInfoWidget::OnSetReadyClicked()
{
	if (OnSetReadyCallback.IsSet())
	{
		OnSetReadyCallback();
	}
}

void ULobbyInfoWidget::OnLeaveLobbyClicked()
{
	Chain::Execute(UBROnlineSubsystem::Get(GetWorld()), &UBROnlineSubsystem::DestroySession);
}

void ULobbyInfoWidget::UpdatePlayerCount()
{
	Chain::Execute(UBROnlineSubsystem::Get(GetWorld()), [this](UBROnlineSubsystem* OnlineSubsystem)
	{
		PlayerCount->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"),
			OnlineSubsystem->GetCurrentPlayerCountSession(), OnlineSubsystem->GetMaxPlayerCountSession())));
	});
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
	UpdatePlayerCount();
}

void ULobbyInfoWidget::UpdatePlayerInfo(ABRLobbyPlayerState* PlayerState)
{
	auto PlayerInfo = PlayerInfoMap[PlayerState];
	PlayerInfo->SetIsReady(PlayerState->GetIsReady());
	PlayerInfo->SetUserName(PlayerState->GetPlayerName());
	UpdatePlayerCount();
}

void ULobbyInfoWidget::RemovePlayerInfo(ABRLobbyPlayerState* PlayerState)
{
	PlayerInfoMap[PlayerState]->RemoveFromParent();
	PlayerInfoMap.Remove(PlayerState);
	UpdatePlayerCount();
}

void ULobbyInfoWidget::UpdateStartingTimer()
{
	if (StartingTimerValue <= 0)
	{
		StartingTimerValue = 0;
		if (GetOwningLocalPlayer()->GetPlayerController(GetWorld())->HasAuthority())
		{
			UpdateTimerHolder.Clear();
			Chain::Execute(GetGameInstance<UBRGameInstance>(), [](UBRGameInstance* GameInstance)
			{
				GameInstance->SetMonsterNumber(1);
			});		
			Chain::Execute(UBROnlineSubsystem::Get(GetWorld()), &UBROnlineSubsystem::StartSession);
		}
	}	
	StartingTimer->SetText(FText::FromString(FString::Printf(TEXT("Starting in %d..."), StartingTimerValue--)));
}

void ULobbyInfoWidget::NativeDestruct()
{
	Super::NativeDestruct();
	
	PlayerInfoMap.Empty();
	for (const auto& Delegate : DelegateMap)
	{
		UEventBus::Remove(this, Delegate.Key, Delegate.Value);
	}
	UpdateTimerHolder.Clear();
}

void ULobbyInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	AddEventBusDelegate(Online_Callback_OnDestroySessionCompleted, [this](const FString& SessionName, bool bWasSuccessful)
	{
		const UGameMapsSettings* MapsSettings = GetDefault<UGameMapsSettings>();
    
		const FSoftObjectPath& DefaultMapPath = MapsSettings->GetGameDefaultMap();

		TSoftObjectPtr<UWorld> StartingMap(DefaultMapPath);

		if (!StartingMap.IsNull())
		{
			bool bAbsolute = true;
			FString Options = TEXT("");
		        
			UGameplayStatics::OpenLevelBySoftObjectPtr(this, StartingMap, bAbsolute, Options);
		}
	});

	AddEventBusDelegate(PlayerState_Callback_IsAllPlayerReady, [this](bool bIsAllReady)
	{
		if (bIsAllReady)
		{
			FTimerParameters TimerParameters;
			TimerParameters.bIsLooping = true;
			TimerParameters.Rate = 1.f;
			TimerParameters.FirstDelay = 0.f;
			StartingTimerValue = 5;
			UpdateTimerHolder.Schedule(this, &ThisClass::UpdateStartingTimer, TimerParameters);
		}
		else
		{
			UpdateTimerHolder.Clear();
			StartingTimerValue = 5;
			StartingTimer->SetText(FText::FromString(TEXT("Waiting for players...")));
		}
	});

	AddEventBusDelegate(PlayerState_Callback_LobbyReadyChange, [this](ABRLobbyPlayerState* PlayerState, bool bIsReady)
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

	AddEventBusDelegate(PlayerState_Callback_LeaveLobby, [this](ABRLobbyPlayerState* PlayerState)
	{
		RemovePlayerInfo(PlayerState);
	});
	
	SetReady->OnClicked.AddDynamic(this, &ThisClass::OnSetReadyClicked);
	LeaveLobby->OnClicked.AddDynamic(this, &ThisClass::OnLeaveLobbyClicked);
	
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
	
	UpdatePlayerCount();
}
