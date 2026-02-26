// Fill out your copyright notice in the Description page of Project Settings.


#include "GameState/BRGameGameState.h"

#include "Chain.h"
#include "EventBus.h"
#include "Linq.h"
#include "GameState/BRGameStateGameplayTags.h"
#include "Items/BRItemGameplayTag.h"
#include "Net/UnrealNetwork.h"



void ABRGameGameState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABRGameGameState, SharedInventory);
	DOREPLIFETIME(ABRGameGameState, bAllObjectiveIsCompleted);
}

void ABRGameGameState::BeginPlay()
{
	Super::BeginPlay();
	UEventBus::LockSignature<const FItemData&>(this, Item_Callback_OnItemPickUp);
}

void ABRGameGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UEventBus::UnlockSignature(this, Item_Callback_OnItemPickUp);
	
}

void ABRGameGameState::AddItem(const FItemData& NewItem)
{
	for (auto& Item : SharedInventory)
	{
		if (Item == NewItem)
		{
			Item.Count += NewItem.Count;
			UEventBus::Broadcast<const FItemData&>(this, Item_Callback_OnItemPickUp, NewItem);
			return;
		}
	}

	SharedInventory.Add(NewItem);
	UEventBus::Broadcast<const FItemData&>(this, Item_Callback_OnItemPickUp, NewItem);
}

void ABRGameGameState::RemoveItem(const FItemData& NewItem)
{
	SharedInventory.RemoveAll([&](const FItemData& Item){return Item == NewItem;});
}

void ABRGameGameState::ClearInventory()
{
	SharedInventory.Empty();
}

TOptional<FItemData> ABRGameGameState::GetItem(const FGameplayTag& Tag) const
{
	return Linq::Start(SharedInventory).First([&](const FItemData& Item)
	{
		return Item.Type.MatchesTagExact(Tag);
	});
}

const TArray<FItemData>& ABRGameGameState::GetSharedInventory() const
{
	return SharedInventory;
}

void ABRGameGameState::OnObjectivesCompleted_Rep() const
{
	UE_LOG(LogTemp,Error, TEXT("All vhs collected!"));
	if (bAllObjectiveIsCompleted)
	{
		UEventBus::Broadcast(this, GameState_Callback_OnObjectivesCompleted);
	}
}