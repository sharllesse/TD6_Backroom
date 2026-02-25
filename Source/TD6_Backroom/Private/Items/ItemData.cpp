// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ItemData.h"

void UItemDataTable::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	const FName ChangedPropertyName = (PropertyChangedEvent.MemberProperty != nullptr) 
			? PropertyChangedEvent.MemberProperty->GetFName() 
			: NAME_None;

	if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(UItemDataTable, ItemsData))
	{
		for (auto [Type, ItemData] : ItemsData)
		{
			ItemData.Type = Type;
			ItemData.Name = Type.GetTagLeafName().ToString();
			ItemData.Count = 0;
		}
	}
}

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
