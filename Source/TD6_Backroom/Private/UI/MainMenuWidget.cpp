// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenuWidget.h"

#include "Components/Button.h"
#include "UI/CreateRoomWidget.h"
#include "UI/UIManagerSubsystem.h"

void UMainMenuWidget::OnCreateRoomClicked()
{
	Chain::StartChain(GetOwningLocalPlayer())
	.Transform([](const ULocalPlayer* LocalPlayer)
	{
		return LocalPlayer->GetSubsystem<UUIManagerSubsystem>();
	})
	.Execute([](UUIManagerSubsystem* UIManager)
	{
		UIManager->PushMenu<UCreateRoomWidget>();
	});	
}

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CreateRoomButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnCreateRoomClicked);
}
