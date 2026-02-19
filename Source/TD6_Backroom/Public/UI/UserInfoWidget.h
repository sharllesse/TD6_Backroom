// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "UserInfoWidget.generated.h"

class UButton;
class UImage;
class UTextBlock;
/**
 * 
 */
UCLASS()
class TD6_BACKROOM_API UUserInfoWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> UserName;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> IsReady;

public:
	virtual void NativeConstruct() override;
	
	void SetUserName(const FString & InUserName) const;
	void SetIsReady(bool bIsReady) const;
};
