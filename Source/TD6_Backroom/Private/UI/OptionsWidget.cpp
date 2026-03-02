// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/OptionsWidget.h"

#include "Chain.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
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
	Chain::Execute(GetGameInstance()->GetSubsystem<UVivoxSubsystem>(), [this, SelectedItem](UVivoxSubsystem* Subsystem)
	{
		Subsystem->SetMicrophone(SelectedItem);
	});
}

void UOptionsWidget::OnListenerChange(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	Chain::Execute(GetGameInstance()->GetSubsystem<UVivoxSubsystem>(), [this, SelectedItem](UVivoxSubsystem* Subsystem)
{
	Subsystem->SetListener(SelectedItem);
});
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
	});
	
	MicrophoneDropDown->OnSelectionChanged.AddDynamic(this, &ThisClass::OnMicrophoneChange);
	ListenerDropDown->OnSelectionChanged.AddDynamic(this, &ThisClass::OnListenerChange);
	
	if (UGameplayStatics::DoesSaveGameExist(OptionSaveSlot, 0))
	{
		if (auto LoadedSettings = Cast<UOptionSettingsSave>(UGameplayStatics::LoadGameFromSlot(OptionSaveSlot, 0)))
		{
			MicrophoneDropDown->SetSelectedOption(LoadedSettings->MicrophoneDevice);
			ListenerDropDown->SetSelectedOption(LoadedSettings->SpeakerDevice);
		}
	}
}

void UOptionsWidget::NativeDestruct()
{
	Super::NativeDestruct();
	auto SaveInstance = Cast<UOptionSettingsSave>(UGameplayStatics::CreateSaveGameObject(UOptionSettingsSave::StaticClass()));
    
	if (SaveInstance != nullptr)
	{
		SaveInstance->MicrophoneDevice = MicrophoneDropDown->GetSelectedOption();
		SaveInstance->SpeakerDevice = ListenerDropDown->GetSelectedOption();
        
		UGameplayStatics::SaveGameToSlot(SaveInstance, OptionSaveSlot, 0);
	}
}
