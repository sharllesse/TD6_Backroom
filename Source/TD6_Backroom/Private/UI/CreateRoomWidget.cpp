// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CreateRoomWidget.h"

#include "OnlineSubsystem.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "Components/CheckBox.h"
#include "Online/BROnlineSubsystem.h"
#include "UI/UIManagerSubsystem.h"

void UCreateRoomWidget::UpdateTextSlider(float NewValue)
{
	TextSlider->SetText(FText::FromString(FString::Printf(TEXT("%d"), static_cast<int>(NewValue))));
}

void UCreateRoomWidget::PopThisMenu()
{
	Chain::Execute(GetOwningLocalPlayer()->GetSubsystem<UUIManagerSubsystem>(), &UUIManagerSubsystem::PopMenu);
}

void UCreateRoomWidget::CreateSession()
{
	Chain::Execute(UBROnlineSubsystem::Get(GetWorld()), [&](UBROnlineSubsystem* Object)
	{
		Object->CreateSession(static_cast<int>(Slider->GetValue()), RoomName->GetText().ToString(), IsPrivateRoom->IsChecked());
	});
}

void UCreateRoomWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Slider->OnValueChanged.AddDynamic(this, &UCreateRoomWidget::UpdateTextSlider);
	BackButton->OnClicked.AddDynamic(this, &UCreateRoomWidget::PopThisMenu);
	CreateRoom->OnClicked.AddDynamic(this, &UCreateRoomWidget::UCreateRoomWidget::CreateSession);
}
