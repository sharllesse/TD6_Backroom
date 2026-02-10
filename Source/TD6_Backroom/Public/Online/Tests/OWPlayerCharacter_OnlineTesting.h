// Copyright (c) Logicraft Interactive. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "OWPlayerCharacter_OnlineTesting.generated.h"

UCLASS()
class OVERWRITE_API AOWPlayerCharacter_OnlineTesting : public ACharacter
{
	GENERATED_BODY()

	friend class AOWPlayerController_OnlineTesting;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<UCameraComponent> CameraComponent;
	
public:
	// Sets default values for this character's properties
	AOWPlayerCharacter_OnlineTesting();

protected:
	void OnMove(const FInputActionValue& InputActionValue);

	void OnJump(const FInputActionValue& InputActionValue);
};
