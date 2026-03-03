// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BRGameInstance.generated.h"

class UOptionSettingsSave;
/**
 * 
 */
UCLASS()
class TD6_BACKROOM_API UBRGameInstance : public UGameInstance
{
	GENERATED_BODY()

protected:
	virtual void Init() override;
	virtual void Shutdown() override;

	void HandleNetworkDisconnect(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);

	UPROPERTY()
	TObjectPtr<UOptionSettingsSave> OptionSettingsSave;

	
	static inline FString OptionSaveSlot{TEXT("Settings")};

	int MonsterNumber{1};
public:

	int GetMonsterNumber() const;
	void SetMonsterNumber(int InMonsterNumber);

	UOptionSettingsSave* GetOptionsSettings();
	void SaveOptionsSettings();
};
