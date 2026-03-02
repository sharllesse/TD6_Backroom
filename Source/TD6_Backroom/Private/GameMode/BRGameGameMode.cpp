// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/BRGameGameMode.h"

#include "Chain.h"
#include "EventBus.h"
#include "Actor/ActorGameplayTags.h"
#include "GameMode/BRGameModeGameplayTags.h"
#include "GameState/BRGameGameState.h"
#include "Items/BRItemGameplayTag.h"

void ABRGameGameMode::BeginPlay()
{
	Super::BeginPlay();

	VhsToCollect = FMath::RandRange(VhsToCollectRange.X, VhsToCollectRange.Y);

	Chain::Execute(GetGameState<ABRGameGameState>(), [this](ABRGameGameState* Game)
	{
		Game->VhsToCollect = VhsToCollect;
	});
	
	DelegateHandler.AddDelegate(this, Item_Callback_OnItemPickUp, [this](const FItemData& Item)
	{
		if (Item.Type.MatchesTagExact(Item_VHS))
			CheckIfHasAllVhs();		
	});

	DelegateHandler.AddDelegate(this, Actor_ExitZone_Callback_OnPlayerEnter, [this](AActor* Player)
	{
		++CurrentPlayerNumberInExitZone;
		CheckIfAllPlayerAreInExitZone();
	});

	DelegateHandler.AddDelegate(this, Actor_ExitZone_Callback_OnPlayerLeave, [this](AActor* Player)
	{
		--CurrentPlayerNumberInExitZone;
	});
}

void ABRGameGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	DelegateHandler.Clear(this);
}

void ABRGameGameMode::CheckIfHasAllVhs()
{
	if (bHasAllVhs)
	{
		return;
	}
	
	Chain::Execute(GetGameState<ABRGameGameState>(), [this](ABRGameGameState* Game)
	{
		if (auto Item = Game->GetItem(Item_VHS))
		{
			if (Item->Count >= VhsToCollect)
			{
				bHasAllVhs = true;
				Game->bAllObjectiveIsCompleted = true;
				Game->OnObjectivesCompleted_Rep();				
			}
		}
	});
}

void ABRGameGameMode::CheckIfAllPlayerAreInExitZone()
{
	if (bAllPlayerAreInExitZone)
	{
		return;
	}
	
	Chain::Execute(GetGameState<ABRGameGameState>(), [this](ABRGameGameState* Game)
	{
		if (CurrentPlayerNumberInExitZone >= Game->PlayerArray.Num())
		{
			bAllPlayerAreInExitZone = true;
			Game->MulticastNotifyAllPlayerAreInExitZone();
		}
	});
	
}

int ABRGameGameMode::GetVhsObjective() const
{
	return VhsToCollect;
}
