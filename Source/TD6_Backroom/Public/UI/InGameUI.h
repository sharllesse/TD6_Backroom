// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "InGameUI.generated.h"

/**
 * 
 */
UCLASS()
class TD6_BACKROOM_API UInGameUI : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> VHSText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> InteractText;

	FDelegateHandle UpdateVHSTextHandle;
	int MaxVHS{0};
	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	void SetCanInteract(bool bCanInteract, const FText& InteractionText) const;
};
