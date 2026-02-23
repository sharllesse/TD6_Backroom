// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstance/BRGameInstance.h"

#include "Items/ItemData.h"

void UBRGameInstance::Init()
{
	Super::Init();

	UItemDataTable::Init(GetWorld());
}

void UBRGameInstance::Shutdown()
{
	Super::Shutdown();
	
	UItemDataTable::Destroy();
}
