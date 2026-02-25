// Copyright (c) Logicraft Interactive. All Rights Reserved.

#include "Character/BRPlayerCharacter.h"

#include "EventBus.h"
#include "Character/BRCharacterGameplayTags.h"

ABRPlayerCharacter::ABRPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm Component"));
	SpringArmComponent->SetupAttachment(GetMesh(), TEXT("head_socket"));

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	CameraComponent->SetupAttachment(SpringArmComponent);
}

void ABRPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (IsLocallyControlled())
	{
		UEventBus::LockSignature<const FTransform&>(this, Character_Callback_OnPlayerMove);
	}
}

void ABRPlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (IsLocallyControlled())
	{
		UEventBus::UnlockSignature(this, Character_Callback_OnPlayerMove);
	}
}

void ABRPlayerCharacter::OnMove(const FInputActionValue& InputActionValue)
{
	const FVector2D MoveInput{InputActionValue.Get<FVector2D>()};

	const FRotator ControlYaw{0.f, GetControlRotation().Yaw, 0.f};
	AddMovementInput(FRotationMatrix(ControlYaw).GetUnitAxis(EAxis::X), MoveInput.Y);
	AddMovementInput(FRotationMatrix(ControlYaw).GetUnitAxis(EAxis::Y), MoveInput.X);
	UEventBus::Broadcast<const FTransform&>(this, Character_Callback_OnPlayerMove, GetActorTransform());
}

void ABRPlayerCharacter::OnJump(const FInputActionValue& InputActionValue)
{
	Jump();
}