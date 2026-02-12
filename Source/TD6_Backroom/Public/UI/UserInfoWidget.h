// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserInfoWidget.generated.h"

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
	TObjectPtr<UTextBlock> Presence;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Activity;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> UserAvatar;

	UPROPERTY(EditAnywhere)
	FVector2D ImageSize;
public:

	void SetActivity(const FString & InActivity) const;
	void SetPresence(const FString & InPresence) const;
	void SetUserName(const FString & InUserName) const;
	void SetUserAvatar(UTexture2DDynamic* InUserAvatar) const;
};
