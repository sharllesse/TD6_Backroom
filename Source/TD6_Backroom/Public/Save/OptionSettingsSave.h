// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "OptionSettingsSave.generated.h"

/**
 * 
 */
UCLASS()
class TD6_BACKROOM_API UOptionSettingsSave : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, Category = "Settings")
	FString MicrophoneDevice;

	UPROPERTY(VisibleAnywhere, Category = "Settings")
	FString SpeakerDevice;

};
