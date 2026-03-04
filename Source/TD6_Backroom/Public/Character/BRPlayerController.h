// Copyright (c) Logicraft Interactive. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputMappingContext.h"
#include "TimerHolder.h"
#include "Character/BRPlayerCharacter.h"
#include "Character/BRSpectatorPawn.h"
#include "Utils/FEventBusDelegateHandler.h"
#include "BRPlayerController.generated.h"

class USpectatorWidget;
class UOptionSettingsSave;
class UVivoxSubsystem;
class UInGameUI;
class UCreateRoomWidget;
class UMainMenuWidget;

USTRUCT(BlueprintType, Blueprintable)
struct FAction
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UInputAction> Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ETriggerEvent TriggerEvent{ETriggerEvent::Triggered};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName FunctionName;
};

/**
 * 
 */
UCLASS()
class TD6_BACKROOM_API ABRPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs|Game")
	TObjectPtr<UInputMappingContext> GameMappingContext;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs|Spectator")
	TObjectPtr<UInputMappingContext> SpectatorMappingContext;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs|Game")
	TArray<FAction> GameInputActions;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs|Spectator")
	TArray<FAction> SpectatorInputActions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	TObjectPtr<USoundCue> GeneralAmbianceSound;

	TWeakObjectPtr<ABRPlayerCharacter> GameCharacter;
	TWeakObjectPtr<ABRSpectatorPawn> CustomSpectatorPawn;

	UPROPERTY(Transient)
	TObjectPtr<UInGameUI> InGameUI;
	
	UPROPERTY(Transient)
	TObjectPtr<USpectatorWidget> SpectatorWidget;
	
	TWeakObjectPtr<UVivoxSubsystem> VivoxSubsystem;
	TWeakObjectPtr<UOptionSettingsSave> SaveSettings;

	FEventBusDelegateHandler DelegateHandler;
	
	bool bIsInPause{false};

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaSeconds) override;
	
	void SetupLocalInfo();
	
	virtual void SetupInputComponent() override;

	void OnPossessPawnLocalLogic(APawn* InPawn);
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnRep_Pawn() override;
	virtual void BeginSpectatingState() override;
	
	virtual void AcknowledgePossession(class APawn* P) override;
	
	void SwitchMappingContext(const FName& Name);

	UFUNCTION()
	void OnMove(const FInputActionValue& InputActionValue) const;

	UFUNCTION()
	void OnLook(const FInputActionValue& InputActionValue);

	UFUNCTION()
	void OnJump(const FInputActionValue& InputActionValue) const;
	
	UFUNCTION()
	void OnInteract(const FInputActionValue& InputActionValue) const;
	
	UFUNCTION()
	void OnCrouch(const FInputActionValue& InputActionValue) const;
	
	UFUNCTION()
	void OnSprint(const FInputActionValue& InputActionValue) const;

	UFUNCTION()
	void OnNextSpectate(const FInputActionValue& InputActionValue);

	void GoToNextSpectator(ABRSpectatorPawn::IterationMethode);
	
	UFUNCTION()
	void OnOpenPauseMenu(const FInputActionValue& InputActionValue);

	UFUNCTION(Server, Reliable)
	void Server_SwitchToSpectator();
	
	UFUNCTION(Client, Reliable)
	void Client_SwitchToSpectator();
public:
	UFUNCTION(BlueprintCallable)
	void RequestSwitchToSpectator();
};
