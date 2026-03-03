// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseWidget.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class TD6_BACKROOM_API UPauseWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ResumeButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> OptionButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QuitButton;

	UFUNCTION()
	void OnResumeClicked();
	UFUNCTION()
	void OnOptionClicked();
	UFUNCTION()
	void OnQuitClicked();

	virtual void NativeConstruct() override;
};
