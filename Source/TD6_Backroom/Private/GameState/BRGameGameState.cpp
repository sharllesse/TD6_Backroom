// Fill out your copyright notice in the Description page of Project Settings.


#include "GameState/BRGameGameState.h"

#include "Net/UnrealNetwork.h"

void ABRGameGameState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABRGameGameState, SharedInventory);
}

void ABRGameGameState::AddItem(const FItemData& NewItem)
{
	for (auto& Item : SharedInventory)
	{
		if (Item == NewItem)
		{
			Item.Count += NewItem.Count;
			return;
		}
	}

	SharedInventory.Add(NewItem);
}

void ABRGameGameState::RemoveItem(const FItemData& NewItem)
{
	SharedInventory.RemoveAll([&](const FItemData& Item){return Item == NewItem;});
}

void ABRGameGameState::ClearInventory()
{
	SharedInventory.Empty();
}
