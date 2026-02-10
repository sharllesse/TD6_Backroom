// Copyright (c) Logicraft Interactive. All Rights Reserved.

#include "Online/Tests/OWPlayerCharacter_OnlineTesting.h"

AOWPlayerCharacter_OnlineTesting::AOWPlayerCharacter_OnlineTesting()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm Component"));
	SpringArmComponent->SetupAttachment(RootComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	CameraComponent->SetupAttachment(SpringArmComponent);
}

void AOWPlayerCharacter_OnlineTesting::OnMove(const FInputActionValue& InputActionValue)
{
	const FVector2D MoveInput{InputActionValue.Get<FVector2D>()};

	const FRotator ControlYaw{0.f, GetControlRotation().Yaw, 0.f};
	AddMovementInput(FRotationMatrix(ControlYaw).GetUnitAxis(EAxis::X), MoveInput.Y);
	AddMovementInput(FRotationMatrix(ControlYaw).GetUnitAxis(EAxis::Y), MoveInput.X);
}

void AOWPlayerCharacter_OnlineTesting::OnJump(const FInputActionValue& InputActionValue)
{
	Jump();
}