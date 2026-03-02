// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OptionsWidget.generated.h"

class UComboBoxString;
class UButton;
/**
 * 
 */
UCLASS()
class TD6_BACKROOM_API UOptionsWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> MicrophoneDropDown;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> ListenerDropDown;
	
	static inline FString OptionSaveSlot{TEXT("Settings")};
	
	UFUNCTION()
	void OnBackButtonClicked();
	
	UFUNCTION()
	void OnMicrophoneChange(FString SelectedItem, ESelectInfo::Type SelectionType);
	
	UFUNCTION()
	void OnListenerChange(FString SelectedItem, ESelectInfo::Type SelectionType);
	
	virtual void NativeConstruct() override;
	
	virtual void NativeDestruct() override;
};
