// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UserInfoWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"


void UUserInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UUserInfoWidget::SetUserName(const FString& InUserName) const
{
	UserName->SetText(FText::FromString(InUserName));
}

void UUserInfoWidget::SetIsReady(bool bIsReady) const
{
	IsReady->SetText(FText::FromString(bIsReady ? TEXT("Ready") : TEXT("Not ready")));
	FSlateColor SlateColor(bIsReady ? FColor::Green : FColor::Red);
	IsReady->SetColorAndOpacity(SlateColor);
}

