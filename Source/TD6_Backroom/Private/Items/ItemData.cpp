// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ItemData.h"

void UItemDataTable::Init(UWorld* WorldContext)
{
	if (!WorldContext)
		return;

	FString AssetPath = TEXT("/Game/Blueprint/Item/DA_ItemDataTable.DA_ItemDataTable");
	
	Self = TStrongObjectPtr(LoadObject<UItemDataTable>(WorldContext, AssetPath));
}

void UItemDataTable::Destroy()
{
	Self.Reset();
}

TOptional<FItemData> UItemDataTable::GetItemData(const FGameplayTag& Type)
{
	if (!Self->ItemsData.Find(Type))
	{
		return NullOpt;
	}

	return Self->ItemsData[Type];
}

const TMap<FGameplayTag, FItemData>& UItemDataTable::GetDataTable()
{
	return Self->ItemsData;
}
