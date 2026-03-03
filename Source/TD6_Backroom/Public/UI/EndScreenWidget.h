// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TimerHolder.h"
#include "Blueprint/UserWidget.h"
#include "EndScreenWidget.generated.h"

class UBorder;
/**
 * 
 */
UCLASS()
class TD6_BACKROOM_API UEndScreenWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> VictoryScreen;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> DefeatScreen;

	FTimerHolder EndGameTimer;
	
	virtual void NativeDestruct() override;
public:
	enum class State
	{
		Win,
		Lose
	};

	void SetState(State InState);
};
