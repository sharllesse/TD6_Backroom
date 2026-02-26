// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/BRGameGameMode.h"

#include "Chain.h"
#include "EventBus.h"
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
	
	UpdateVhsStateDelegate = UEventBus::AddLambda(this, Item_Callback_OnItemPickUp, [this](const FItemData& Item)
	{
		if (Item.Type.MatchesTagExact(Item_VHS))
			CheckIfHasAllVhs();		
	});
}

void ABRGameGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UEventBus::Remove(this, Item_Callback_OnItemPickUp, UpdateVhsStateDelegate);
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

int ABRGameGameMode::GetVhsObjective() const
{
	return VhsToCollect;
}
