// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OptionsWidget.generated.h"

class UTextBlock;
class USlider;
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

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> MouseSensibility;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextMouseSensibility;
	
	UFUNCTION()
	void OnBackButtonClicked();
	
	UFUNCTION()
	void OnMicrophoneChange(FString SelectedItem, ESelectInfo::Type SelectionType);
	void SetMicrophone(const FString& SelectedDevice);
	
	UFUNCTION()
	void OnListenerChange(FString SelectedItem, ESelectInfo::Type SelectionType);
	void SetListener(const FString& SelectedDevice);

	UFUNCTION()
	void OnMouseSensibilityChange(float NewValue);
	
	virtual void NativeConstruct() override;
	
	virtual void NativeDestruct() override;
};
