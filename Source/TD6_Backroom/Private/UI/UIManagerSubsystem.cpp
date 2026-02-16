// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UIManagerSubsystem.h"

void UUIManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FString AssetPath = TEXT("/Game/Blueprint/UI/DA_UIManager_Data.DA_UIManager_Data");

	ManagerData = LoadObject<UUIManager_Data>(nullptr, *AssetPath);

	if (ManagerData)
	{
		UE_LOG(LogTemp, Log, TEXT("UI manager data asset loaded: %s"), *ManagerData->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load UI manager data asset at path: %s"), *AssetPath);
	}
	
	
}
