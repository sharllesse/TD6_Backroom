// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/OptionsWidget.h"

#include "Chain.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "GameInstance/BRGameInstance.h"
#include "GameInstance/Subsystem/VivoxSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Save/OptionSettingsSave.h"
#include "UI/UIManagerSubsystem.h"

void UOptionsWidget::OnBackButtonClicked()
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
}

void UOptionsWidget::OnMicrophoneChange(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	SetMicrophone(SelectedItem);
}

void UOptionsWidget::SetMicrophone(const FString& SelectedDevice)
{
	Chain::Execute(GetGameInstance()->GetSubsystem<UVivoxSubsystem>(), [this, &SelectedDevice](UVivoxSubsystem* Subsystem)
	{
		Subsystem->SetMicrophone(SelectedDevice);
	});
}

void UOptionsWidget::OnListenerChange(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	SetListener(SelectedItem);
}

void UOptionsWidget::SetListener(const FString& SelectedDevice)
{
	Chain::Execute(GetGameInstance()->GetSubsystem<UVivoxSubsystem>(), [this, &SelectedDevice](UVivoxSubsystem* Subsystem)
	{
		Subsystem->SetListener(SelectedDevice);
	});
}

void UOptionsWidget::OnMouseSensibilityChange(float NewValue)
{
	TextMouseSensibility->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), NewValue)));
}

void UOptionsWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	BackButton->OnClicked.AddDynamic(this, &ThisClass::OnBackButtonClicked);
	MicrophoneDropDown->ClearOptions();
	
	Chain::Execute(GetGameInstance()->GetSubsystem<UVivoxSubsystem>(), [this](UVivoxSubsystem* Subsystem)
	{
		for (auto Element : Subsystem->GetAllMicrophones())
		{
			MicrophoneDropDown->AddOption(Element);
		}
		
		for (auto Element : Subsystem->GetAllListeners())
		{
			ListenerDropDown->AddOption(Element);
		}
		
		MicrophoneDropDown->SetSelectedOption(Subsystem->GetActiveMicrophone());
		ListenerDropDown->SetSelectedOption(Subsystem->GetActiveListener());
	});
	
	MicrophoneDropDown->OnSelectionChanged.AddDynamic(this, &ThisClass::OnMicrophoneChange);
	ListenerDropDown->OnSelectionChanged.AddDynamic(this, &ThisClass::OnListenerChange);
	MouseSensibility->OnValueChanged.AddDynamic(this, &UOptionsWidget::OnMouseSensibilityChange);
	
	Chain::Execute(GetGameInstance<UBRGameInstance>(), [this](UBRGameInstance* GameInstance)
	{
		if (auto Settings = GameInstance->GetOptionsSettings())
		{
			MicrophoneDropDown->SetSelectedOption(Settings->MicrophoneDevice);
			SetMicrophone(Settings->MicrophoneDevice);
			ListenerDropDown->SetSelectedOption(Settings->SpeakerDevice);
			SetListener(Settings->SpeakerDevice);
			MouseSensibility->SetValue(Settings->MouseSensibility);
		}
	});

}

void UOptionsWidget::NativeDestruct()
{
	Super::NativeDestruct();
	Chain::Execute(GetGameInstance<UBRGameInstance>(), [this](UBRGameInstance* GameInstance)
	{
		auto SaveInstance = GameInstance->GetOptionsSettings();
	    
		if (SaveInstance != nullptr)
		{
			SaveInstance->MicrophoneDevice = MicrophoneDropDown->GetSelectedOption();
			SaveInstance->SpeakerDevice = ListenerDropDown->GetSelectedOption();
			SaveInstance->MouseSensibility = MouseSensibility->GetValue();
		}
		GameInstance->SaveOptionsSettings();
	});
	
}
