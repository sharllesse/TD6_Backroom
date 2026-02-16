// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CreateRoomWidget.generated.h"

class UButton;
class UCheckBox;
class UTextBlock;
class USlider;
class UEditableTextBox;
/**
 * 
 */
UCLASS()
class TD6_BACKROOM_API UCreateRoomWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> RoomName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> Slider;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextSlider;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCheckBox> IsPrivateRoom;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CreateRoom;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackButton;

	UFUNCTION()
	void UpdateTextSlider(float NewValue);

	UFUNCTION()
	void PopThisMenu();

	UFUNCTION()
	void CreateSession();
public:

	virtual void NativeConstruct() override;
};
