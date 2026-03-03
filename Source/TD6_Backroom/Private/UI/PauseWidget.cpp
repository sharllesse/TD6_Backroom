// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PauseWidget.h"
#include "UI/OptionsWidget.h"
#include "UI/UIManagerSubsystem.h"
#include "Chain.h"
#include "Components/Button.h"
#include "Online/BROnlineSubsystem.h"

class UOptionsWidget;
class UUIManagerSubsystem;

void UPauseWidget::OnResumeClicked()
{	
	Chain::StartChain(GetOwningLocalPlayer())
	.Transform([](const ULocalPlayer* LocalPlayer)
	{
		return LocalPlayer->GetSubsystem<UUIManagerSubsystem>();
	})
	.Execute([](UUIManagerSubsystem* UIManager)
	{
		UIManager->PopMenu();
	});
	
	Chain::StartChain(GetOwningLocalPlayer())
	.Transform([this](const ULocalPlayer* LocalPlayer)
	{
		return LocalPlayer->GetPlayerController(GetWorld());
	})
	.Execute([](APlayerController* PlayerController)
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
		PlayerController->bShowMouseCursor = false;
	});
}

void UPauseWidget::OnOptionClicked()
{
	Chain::StartChain(GetOwningLocalPlayer())
	.Transform([](const ULocalPlayer* LocalPlayer)
	{
		return LocalPlayer->GetSubsystem<UUIManagerSubsystem>();
	})
	.Execute([](UUIManagerSubsystem* UIManager)
	{
		UIManager->PushMenu<UOptionsWidget>();
	});	
}

void UPauseWidget::OnQuitClicked()
{
	Chain::Execute(UBROnlineSubsystem::Get(GetWorld()), [](UBROnlineSubsystem* Subsystem)
	{
		Subsystem->DestroySession();
	});
}

void UPauseWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ResumeButton->OnClicked.AddDynamic(this, &UPauseWidget::OnResumeClicked);
	OptionButton->OnClicked.AddDynamic(this, &UPauseWidget::OnOptionClicked);
	QuitButton->OnClicked.AddDynamic(this, &UPauseWidget::OnQuitClicked);
}
