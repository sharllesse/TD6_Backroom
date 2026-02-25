// Copyright (c) Logicraft Interactive. All Rights Reserved.

#include "Character/BRPlayerCharacter.h"

#include "Chain.h"
#include "EventBus.h"
#include "ActorComponent/InteractionComponent.h"
#include "Character/BRCharacterGameplayTags.h"
#include "GameState/BRGameGameState.h"

ABRPlayerCharacter::ABRPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm Component"));
	SpringArmComponent->SetupAttachment(GetMesh(), TEXT("head_socket"));

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("Interaction System"));
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

void ABRPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	Chain::Execute(GetWorld()->GetGameState<ABRGameGameState>(), [&](ABRGameGameState* GameState)
			{
				for (int i = 0; i < GameState->GetSharedInventory().Num(); ++i)
				{
					const auto& Item = GameState->GetSharedInventory()[i];
					GEngine->AddOnScreenDebugMessage(i, 2.f, FColor::Red,
						FString::Printf(TEXT("Item %d : name %s, count %d"), i, *Item.Name, Item.Count) );
				}
			});
}

void ABRPlayerCharacter::OnTryInteract() const
{
	if (InteractionComponent->CanInteract())
	{
		InteractionComponent->TryInteract(InteractionComponent->GetTarget());
	}
}

void ABRPlayerCharacter::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	OutLocation = CameraComponent->GetComponentLocation();
	OutRotation = GetBaseAimRotation();
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