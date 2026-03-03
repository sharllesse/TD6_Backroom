// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstance/BRGameInstance.h"

#include "Chain.h"
#include "Items/ItemData.h"
#include "Kismet/GameplayStatics.h"
#include "Online/BROnlineSubsystem.h"
#include "Save/OptionSettingsSave.h"

void UBRGameInstance::Init()
{
	Super::Init();

	UItemDataTable::Init(GetWorld());

	if (GEngine)
	{
		GEngine->OnNetworkFailure().AddUObject(this, &ThisClass::HandleNetworkDisconnect);
	}

	if (UGameplayStatics::DoesSaveGameExist(OptionSaveSlot, 0))
	{
		OptionSettingsSave = Cast<UOptionSettingsSave>(UGameplayStatics::LoadGameFromSlot(OptionSaveSlot, 0));
	}
	else
	{
		OptionSettingsSave = Cast<UOptionSettingsSave>(UGameplayStatics::CreateSaveGameObject(UOptionSettingsSave::StaticClass()));
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

UOptionSettingsSave* UBRGameInstance::GetOptionsSettings()
{
	return OptionSettingsSave;
}

void UBRGameInstance::SaveOptionsSettings()
{
	if (OptionSettingsSave)
	{
		UGameplayStatics::SaveGameToSlot(OptionSettingsSave, OptionSaveSlot, 0);
	}
}
