// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UserInfoWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UUserInfoWidget::SetActivity(const FString& InActivity) const
{
	Activity->SetText(FText::FromString(InActivity));
}

void UUserInfoWidget::SetPresence(const FString& InPresence) const
{
	Presence->SetText(FText::FromString(InPresence));
}

void UUserInfoWidget::SetUserName(const FString& InUserName) const
{
	UserName->SetText(FText::FromString(InUserName));
}

void UUserInfoWidget::SetUserAvatar(UTexture2DDynamic* InUserAvatar) const
{
	if (!InUserAvatar)
		return;

	UserAvatar->SetBrushFromTextureDynamic(InUserAvatar);
	UserAvatar->SetDesiredSizeOverride(ImageSize);
}
