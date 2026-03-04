// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SpectatorWidget.h"

#include "Character/BRCharacterGameplayTags.h"
#include "Components/Button.h" 
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

void USpectatorWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	DelegateHandler.AddDelegate(this, Character_Callback_OnPlayerSwitchSpectate, [this](const APlayerState* PlayerState)
	{
                    UE_LOG(LogTemp, Error, TEXT("Reception"));
		SpectatedPlayer->SetText(FText::FromString(FString::Printf(TEXT("%s"), *PlayerState->GetPlayerName())));
	});
}

void USpectatorWidget::NativeDestruct()
{
	Super::NativeDestruct();
	DelegateHandler.Clear(this);
} 
