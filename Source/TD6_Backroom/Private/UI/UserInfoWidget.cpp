// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UserInfoWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UUserInfoWidget::JoinButtonCallback() const
{
	if (JoinButtonOnClickedEvent.IsSet())
	{
		JoinButtonOnClickedEvent();
	}
}

void UUserInfoWidget::InviteButtonCallback() const
{
	if (InviteButtonOnClickedEvent.IsSet())
	{
		InviteButtonOnClickedEvent();
	}
}

void UUserInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UUserInfoWidget::SetActivity(const FString& InActivity) const
{
	Activity->SetText(FText::FromString(InActivity));
}

void UUserInfoWidget::SetPresence(const FString& InPresence, const FColor& ColorPresence) const
{
	Presence->SetText(FText::FromString(InPresence));
	FSlateColor SlateColor(ColorPresence);
	Presence->SetColorAndOpacity(SlateColor);
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

void UUserInfoWidget::SetCanBeJoin(bool bCanJoin) const
{
	if (bCanJoin)
	{
		JoinButton->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		JoinButton->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UUserInfoWidget::SetCanBeInvited(bool bCanInvite) const
{
	if (bCanInvite)
	{
		InviteButton->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		InviteButton->SetVisibility(ESlateVisibility::Collapsed);
	}
}

