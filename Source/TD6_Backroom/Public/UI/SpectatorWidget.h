// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Utils/FEventBusDelegateHandler.h"
#include "SpectatorWidget.generated.h"

class UButton;
class UTextBlock;

/**
 * 
 */
UCLASS()
class TD6_BACKROOM_API USpectatorWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> SpectatedPlayer;
	
	FEventBusDelegateHandler DelegateHandler;
	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
};
