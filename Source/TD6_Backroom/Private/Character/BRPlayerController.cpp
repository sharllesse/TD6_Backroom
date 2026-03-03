// Copyright (c) Logicraft Interactive. All Rights Reserved.

#include "Character/BRPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "EventBus.h"
#include "GameFramework/PlayerState.h"
#include "GameInstance/BRGameInstance.h"
#include "GameInstance/Subsystem/VivoxSubsystem.h"
#include "GameMode/BRGameModeGameplayTags.h"
#include "GameState/BRGameStateGameplayTags.h"
#include "Online/BROnlineGameTags.h"
#include "Perception/AIPerceptionSystem.h"
#include "PlayerState/BRGamePlayerState.h"
#include "PlayerState/BRPlayerStateGameTags.h"
#include "Save/OptionSettingsSave.h"
#include "UI/EndScreenWidget.h"
#include "UI/UIManagerSubsystem.h"
#include "UI/InGameUI.h"
#include "UI/OptionsWidget.h"
#include "UI/EndScreenWidget.h"
#include "UI/PauseWidget.h"


void ABRPlayerController::BeginPlay()
{
	Super::BeginPlay();
	 
	

	if (IsLocalController())
	{
		SetupLocalInfo();
	}
	

	UEventBus::AddLambda(this, GameMode_Callback_OnAllPlayerInExitZone, []
		{
			GEngine->AddOnScreenDebugMessage(0, 2.f, FColor::Red, TEXT("All player in exit zone."));
		});
	
	if (IsLocalController())
	{
		VivoxSubsystem = GetGameInstance()->GetSubsystem<UVivoxSubsystem>();
		if (VivoxSubsystem.IsValid())
		{
			UE_LOG(Log_BRVivox, Error, TEXT("Switching to 3d room"))
			VivoxSubsystem->SwitchTo3DRoom();	
		}
	}

	Chain::Execute(GetGameInstance<UBRGameInstance>(), [this](UBRGameInstance* GameInstance)
	{
		SaveSettings = GameInstance->GetOptionsSettings();
	});
	
	//UEventBus::AddUObject(this, Online_Callback_OnExternalUIChange, this, &ABRPlayerController::OnExternalUIChange);
}

void ABRPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	DelegateHandler.Clear(GetWorld());
}

void ABRPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (IsLocalController())
	{
		if (!VivoxSubsystem.IsValid())
		{
			VivoxSubsystem = GetGameInstance()->GetSubsystem<UVivoxSubsystem>();
		}
	
		if (VivoxSubsystem.IsValid() && IsValid(GetPawn()))
		{
			VivoxSubsystem->Set3DPosition(GetPawn()->GetActorLocation(), GetPawn()->GetActorForwardVector(), GetPawn()->GetActorUpVector());	
		}
	}
}

void ABRPlayerController::SetupLocalInfo()
{
	SetInputMode(FInputModeGameOnly());
	bShowMouseCursor = false; 
	
		
	UEventBus::AddLambda(this, Online_Callback_OnMainExternalUIOverlayChange, [this](bool bIsOpening)
	{
		if (bIsOpening)
		{
			SetInputMode(FInputModeUIOnly());
			bShowMouseCursor = true; 
		}
		else
		{
			SetInputMode(FInputModeGameOnly());
			bShowMouseCursor = false; 
		}
	});

	InGameUI = GetLocalPlayer()->GetSubsystem<UUIManagerSubsystem>()->CreateWidget<UInGameUI>();
	GameCharacter->SetNotifyInteractCallback([this](bool bCanInteract,const IInteractable* Interactable)
	{
		if (Interactable)
		{
			InGameUI->SetCanInteract(bCanInteract, Interactable->GetInteractText());
		}
		else
		{
			InGameUI->SetCanInteract(bCanInteract, FText::FromString(TEXT("")));
		}
	});

	GameCharacter->StaminaComponent->OnStaminaChange.BindLambda([this](float Current, float Max)
	{
		InGameUI->SetStaminaBar(Current, Max);
	});

	DelegateHandler.AddDelegate(this, GameMode_Callback_OnAllPlayerInExitZone, [this]
	{
		SetInputMode(FInputModeUIOnly());
		Chain::StartChain(GetLocalPlayer())
		.Transform([](const ULocalPlayer* LocalPlayer)
		{
			return LocalPlayer->GetSubsystem<UUIManagerSubsystem>();
		})
		.Transform([](UUIManagerSubsystem* UIManager)
		{
			return UIManager->CreateWidget<UEndScreenWidget>();
		}).Execute([](UEndScreenWidget* ScreenWidget)
		{
			ScreenWidget->SetState(UEndScreenWidget::State::Win);
		});	
	});
	
	DelegateHandler.AddDelegate(this, GameMode_Callback_OnAllPlayerAreDead, [this]
	{
		SetInputMode(FInputModeUIOnly());
		Chain::StartChain(GetLocalPlayer())
		.Transform([](const ULocalPlayer* LocalPlayer)
		{
			return LocalPlayer->GetSubsystem<UUIManagerSubsystem>();
		})
		.Transform([](UUIManagerSubsystem* UIManager)
		{
			return UIManager->CreateWidget<UEndScreenWidget>();
		})
		.Execute([](UEndScreenWidget* ScreenWidget)
		{
			ScreenWidget->SetState(UEndScreenWidget::State::Lose);
		});	
	});
}

void ABRPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(InputComponent))
	{
		for (FAction& InputAction : GameInputActions)
		{
			Input->BindAction(InputAction.Action, InputAction.TriggerEvent, this, InputAction.FunctionName);
		}
		
		for (FAction& InputAction : SpectatorInputActions)
		{
			Input->BindAction(InputAction.Action, InputAction.TriggerEvent, this, InputAction.FunctionName);
		}

		return;
	}
	
	UE_LOG(LogTemp, Error, TEXT("ABRPlayerController: The input component is null."))
}

void ABRPlayerController::OnPossessPawnLocalLogic(APawn* InPawn)
{
	if (auto IsGameCharacter = Cast<ABRPlayerCharacter>(InPawn))
	{
		GameCharacter = IsGameCharacter;
		SwitchMappingContext(NAME_Playing);
	}
	else if (auto IsSpectatorCharacter = Cast<ABRSpectatorPawn>(InPawn))
	{
		CustomSpectatorPawn = IsSpectatorCharacter;
		SwitchMappingContext(NAME_Spectating);
		if (VivoxSubsystem.IsValid())
		{
			VivoxSubsystem->SwitchTo2DRoom();	
		}
	}
}

void ABRPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (IsLocalPlayerController())
	{
		OnPossessPawnLocalLogic(InPawn);
	}	
}

void ABRPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();
	OnPossessPawnLocalLogic(GetPawn());
}

void ABRPlayerController::AcknowledgePossession(class APawn* P)
{
	Super::AcknowledgePossession(P);
	OnPossessPawnLocalLogic(P);
}


void ABRPlayerController::SwitchMappingContext(const FName& Name)
{
	if (ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player))
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			InputSystem->ClearAllMappings();
			if (Name == NAME_Playing)
			{
				if (GameMappingContext)
				{
					InputSystem->AddMappingContext(GameMappingContext, 0);
					UE_LOG(LogTemp, Error, TEXT("Switching to Game mapping context"))
					return;
				}

				UE_LOG(LogTemp, Error, TEXT("ABRPlayerController: The mapping context cannot be null."))
			}
			else if (Name == NAME_Spectating)
			{
				if (SpectatorMappingContext)
				{
					InputSystem->AddMappingContext(SpectatorMappingContext, 0);
					UE_LOG(LogTemp, Error, TEXT("Switching to Spectator mapping context"))
					return;
				}

				UE_LOG(LogTemp, Error, TEXT("ABRPlayerController: The mapping context cannot be null."))
			}
		}
	}
}

void ABRPlayerController::OnMove(const FInputActionValue& InputActionValue) const
{
	if (!GameCharacter.IsValid())
		return;
	GameCharacter->OnMove(InputActionValue);
}

void ABRPlayerController::OnLook(const FInputActionValue& InputActionValue)
{
	const FVector2D LookInput{InputActionValue.Get<FVector2D>()};
	
	AddPitchInput(LookInput.Y * SaveSettings->MouseSensibility);
	AddYawInput(LookInput.X * SaveSettings->MouseSensibility);
}

void ABRPlayerController::OnJump(const FInputActionValue& InputActionValue) const
{
	if (!GameCharacter.IsValid())
		return;
	GameCharacter->OnJump(InputActionValue);
}

void ABRPlayerController::OnInteract(const FInputActionValue& InputActionValue) const
{
	if (!GameCharacter.IsValid())
		return;
	GameCharacter->OnTryInteract();
}

void ABRPlayerController::OnCrouch(const FInputActionValue& InputActionValue) const
{
	if (!GameCharacter.IsValid())
		return;
	
	if (InputActionValue.Get<bool>())
	{ 
		GameCharacter->Crouch();
	}
	else
	{ 
		GameCharacter->UnCrouch();
	}
}

void ABRPlayerController::OnSprint(const FInputActionValue& InputActionValue) const
{
	if (!GameCharacter.IsValid())
		return;
	GameCharacter->OnSprint(InputActionValue);
}

void ABRPlayerController::OnNextSpectate(const FInputActionValue& InputActionValue)
{
	if (!CustomSpectatorPawn.IsValid())
		return;
	if (!IsLocalController())
		return;
	
	UE_LOG(LogTemp, Error, TEXT("Next spectate"))
	
	if (InputActionValue.Get<bool>())
	{
		Chain::Execute(CustomSpectatorPawn.Get(), [](ABRSpectatorPawn* SpecPawn)
		{
			SpecPawn->SpectateNextPlayer(ABRSpectatorPawn::IterationMethode::Next);
		});
	}
	else
	{
		Chain::Execute(CustomSpectatorPawn.Get(), [](ABRSpectatorPawn* SpecPawn)
		{
			SpecPawn->SpectateNextPlayer(ABRSpectatorPawn::IterationMethode::Previous);
		});
	}
}

void ABRPlayerController::OnOpenPauseMenu(const FInputActionValue& InputActionValue)
{	
	SetInputMode(FInputModeUIOnly());
	bShowMouseCursor = true;
	Chain::StartChain(GetLocalPlayer())
	.Transform([](const ULocalPlayer* LocalPlayer)
	{
		return LocalPlayer->GetSubsystem<UUIManagerSubsystem>();
	})
	.Execute([](UUIManagerSubsystem* UIManager)
	{
		UIManager->PushMenu<UPauseWidget>();
	});	
}


void ABRPlayerController::Client_SwitchToSpectator_Implementation()
{
	UE_LOG(LogTemp, Error, TEXT("Switching to Spectator client"))
	GameCharacter = nullptr;
	ChangeState(NAME_Spectating);

	if (HasAuthority())
		OnRep_Pawn();
}

void ABRPlayerController::Server_SwitchToSpectator_Implementation()
{
	
	UE_LOG(LogTemp, Error, TEXT("Switching to Spectator server"))

	UnPossess();
	
	// if (auto DyingPawn = Cast<ABRPlayerCharacter>(GetPawn()))
	// {
	// 	DyingPawn->EnableRagdoll();
	// }

	// if (UAIPerceptionSystem* PerceptionSystem = UAIPerceptionSystem::GetCurrent(GetWorld()))
	// {
	// 	PerceptionSystem->Upda(); // re-scanne tous les listeners
	// }
	
	if (auto GamePlayerState = Cast<ABRGamePlayerState>(PlayerState))
	{
		GamePlayerState->SetToSpectator();
	}

	ClientGotoState(NAME_Spectating);
	ChangeState(NAME_Spectating);
	
	Client_SwitchToSpectator();
}

void ABRPlayerController::RequestSwitchToSpectator()
{
	UE_LOG(LogTemp, Error, TEXT("Request switch to spectator"))
	Server_SwitchToSpectator();
}

