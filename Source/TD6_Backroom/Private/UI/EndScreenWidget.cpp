// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/EndScreenWidget.h"

#include "Chain.h"
#include "Components/Border.h"
#include "GameInstance/BRGameInstance.h"
#include "Online/BROnlineSubsystem.h"

void UEndScreenWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UEndScreenWidget::SetState(State InState)
{
	if (InState == State::Lose)
	{
		DefeatScreen->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		VictoryScreen->SetVisibility(ESlateVisibility::Visible);
	}

	EndGameTimer.Schedule([this, InState]
	{
		Chain::Execute(UBROnlineSubsystem::Get(GetWorld()), [this,InState](UBROnlineSubsystem* Subsystem)
		{
			if (InState == State::Lose)
			{
				Subsystem->DestroySession();
			}
			else
			{
				if (!Subsystem->IsHost())
				{
					return;
				}
				
				Chain::Execute(GetGameInstance<UBRGameInstance>(), [](UBRGameInstance* GameInstance)
				{
					GameInstance->SetMonsterNumber(GameInstance->GetMonsterNumber() + 1);
				});
				Subsystem->OpenGameWorld();
			}
		});			
	}, {.bIsLooping = false, .Rate = 0.001f ,.FirstDelay = 3.f});
	
}
