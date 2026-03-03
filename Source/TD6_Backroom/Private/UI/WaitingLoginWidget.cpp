// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WaitingLoginWidget.h"

#include "Chain.h"
#include "Components/Button.h"
#include "Online/BROnlineSubsystem.h"


void UWaitingLoginWidget::OnRetryLoginClicked()
{
	Chain::Execute(UBROnlineSubsystem::Get(GetWorld()), [](UBROnlineSubsystem* Subsystem)
	{
		(void)Subsystem->Login();
	});
}

void UWaitingLoginWidget::NativeConstruct()
{
	Super::NativeConstruct();
	RetryLogin->OnClicked.AddDynamic(this, &UWaitingLoginWidget::OnRetryLoginClicked);
}
