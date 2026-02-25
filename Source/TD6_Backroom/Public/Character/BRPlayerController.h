// Copyright (c) Logicraft Interactive. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputMappingContext.h"
#include "TimerHolder.h"
#include "Character/BRPlayerCharacter.h"
#include "BRPlayerController.generated.h"

class UCreateRoomWidget;
class UMainMenuWidget;

USTRUCT(BlueprintType, Blueprintable)
struct FAction
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UInputAction> Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ETriggerEvent TriggerEvent;

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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs")
	TObjectPtr<UInputMappingContext> InputMappingContext;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs")
	TArray<FAction> InputActions;

	TWeakObjectPtr<ABRPlayerCharacter> OwningCharacter;
	

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void SetupInputComponent() override;

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnRep_Pawn() override;
	
	void SetupInputMappingContext();

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
// 	UFUNCTION(BlueprintCallable)
// 	void OnCreateSession_Debug();
// private:
// 	void OnExternalUIChange(bool bIsOpening);
};
