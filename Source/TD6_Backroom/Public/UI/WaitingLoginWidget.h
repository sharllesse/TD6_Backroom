// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WaitingLoginWidget.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class TD6_BACKROOM_API UWaitingLoginWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RetryLogin;

	UFUNCTION()
	void OnRetryLoginClicked();

	virtual void NativeConstruct() override;
};
