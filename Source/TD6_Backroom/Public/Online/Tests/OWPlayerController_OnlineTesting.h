// Copyright (c) Logicraft Interactive. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputMappingContext.h"
#include "Online/Tests/OWPlayerCharacter_OnlineTesting.h"
#include "OWPlayerController_OnlineTesting.generated.h"

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
class OVERWRITE_API AOWPlayerController_OnlineTesting : public APlayerController
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs")
	TObjectPtr<UInputMappingContext> InputMappingContext;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs")
	TArray<FAction> InputActions;

	TWeakObjectPtr<AOWPlayerCharacter_OnlineTesting> OwningCharacter;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void SetupInputComponent() override;

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnRep_Pawn() override;
	
	void SetupInputMappingContext();

	UFUNCTION()
	void OnMove(const FInputActionValue& InputActionValue);

	UFUNCTION()
	void OnLook(const FInputActionValue& InputActionValue);

	UFUNCTION()
	void OnJump(const FInputActionValue& InputActionValue);

	UFUNCTION(BlueprintCallable)
	void OnCreateSession_Debug();
private:
	void OnExternalUIChange(bool bIsOpening);
};
