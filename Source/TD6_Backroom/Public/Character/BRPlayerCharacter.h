// Copyright (c) Logicraft Interactive. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameInstance/Subsystem/VivoxSubsystem.h"
#include "BRPlayerCharacter.generated.h"

class UInteractionComponent;

UCLASS()
class TD6_BACKROOM_API ABRPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	friend class ABRPlayerController;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<UInteractionComponent> InteractionComponent;
public:
	// Sets default values for this character's properties
	ABRPlayerCharacter();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaSeconds) override;
protected:
	void OnMove(const FInputActionValue& InputActionValue);

	void OnJump(const FInputActionValue& InputActionValue);
	
	void OnTryInteract() const;

	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;
};
