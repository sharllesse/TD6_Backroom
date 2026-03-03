// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstance/BRGameInstance.h"

#include "Chain.h"
#include "Items/ItemData.h"
#include "Online/BROnlineSubsystem.h"

void UBRGameInstance::Init()
{
	Super::Init();

	UItemDataTable::Init(GetWorld());

	if (GEngine)
	{
		GEngine->OnNetworkFailure().AddUObject(this, &ThisClass::HandleNetworkDisconnect);
	}
}

void UBRGameInstance::Shutdown()
{
	Super::Shutdown();
	
	UItemDataTable::Destroy();
}

void UBRGameInstance::HandleNetworkDisconnect(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType,
	const FString& ErrorString)
{
	if (FailureType == ENetworkFailure::ConnectionLost || FailureType == ENetworkFailure::FailureReceived)
	{
		Chain::Execute(UBROnlineSubsystem::Get(GetWorld()), [](UBROnlineSubsystem* Subsystem)
		{
			Subsystem->DestroySession();
		});
	}
}
