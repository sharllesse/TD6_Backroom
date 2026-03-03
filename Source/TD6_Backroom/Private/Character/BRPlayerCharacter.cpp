// Copyright (c) Logicraft Interactive. All Rights Reserved.

#include "Character/BRPlayerCharacter.h"

#include "EventBus.h"
#include "ActorComponent/InteractionComponent.h"
#include "Character/BRCharacterGameplayTags.h"
#include "Character/BRPlayerController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AIPerceptionSystem.h"

ABRPlayerCharacter::ABRPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm Component"));
	SpringArmComponent->SetupAttachment(GetMesh(), TEXT("head_socket"));

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("Interaction System"));
	GenericTeamId = 0;
	StaminaComponent = CreateDefaultSubobject<UStaminaComponent>(TEXT("Stamina Component"));

	
}

void ABRPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (UAIPerceptionSystem* PerceptionSystem = UAIPerceptionSystem::GetCurrent(GetWorld()))
	{
		PerceptionSystem->UnregisterSource(*this);
	}
	if (IsLocallyControlled())
	{
		UEventBus::LockSignature<const FTransform&>(this, Character_Callback_OnPlayerMove);

		StaminaComponent->OnCantSprint.BindLambda([this]
		{
			Server_OnSprint(false);
			UpdateSprintState(false);
		});
		StaminaComponent->OnStaminaChange.BindLambda([this](float Current, float Max)
		{
			GEngine->AddOnScreenDebugMessage(0, 1.f, FColor::Red, FString::Printf(TEXT("%.2f/%.2f"),
				Current,Max));
		});
	}

	GetCharacterMovement()->MaxWalkSpeed = PlayerData->WalkSpeed;
	GetCharacterMovement()->MaxAcceleration = PlayerData->WalkSpeed * PlayerData->AccelerationModifier;
	GetCharacterMovement()->MaxWalkSpeedCrouched = PlayerData->CrouchSpeed;

	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

void ABRPlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (IsLocallyControlled())
	{
		UEventBus::UnlockSignature(this, Character_Callback_OnPlayerMove);
	}
}

void ABRPlayerCharacter::SetGenericTeamId(const FGenericTeamId& TeamID)
{
	GenericTeamId = TeamID;
}

FGenericTeamId ABRPlayerCharacter::GetGenericTeamId() const
{
	return GenericTeamId;
}

void ABRPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
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

void ABRPlayerCharacter::EnableRagdoll_Implementation()
{
	if (UCapsuleComponent* CapsuleComp = GetCapsuleComponent())
	{
		CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetCollisionProfileName(FName("Ragdoll"));
		MeshComp->SetSimulatePhysics(true);
		MeshComp->WakeAllRigidBodies();
	}

	if (auto PlayerController = Cast<ABRPlayerController>(Controller))
	{
		PlayerController->RequestSwitchToSpectator();
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

void ABRPlayerCharacter::OnSprint(const FInputActionValue& InputActionValue)
{
	if (!IsLocallyControlled())
	{
		return;
	}

	const bool bWantSprint = InputActionValue.Get<bool>() && !IsCrouched() && StaminaComponent->CanSprint();
	Server_OnSprint(bWantSprint);
	UpdateSprintState(bWantSprint);
}

void ABRPlayerCharacter::Server_OnSprint_Implementation(bool bWantSprint)
{
	UpdateSprintState(bWantSprint);
}

void ABRPlayerCharacter::UpdateSprintState(bool bShouldSprint)
{
	if(bShouldSprint)
	{
		PlayerData->bIsSprinting = true;
		StaminaComponent->SetIsSprinting(true);
		GetCharacterMovement()->MaxWalkSpeed = PlayerData->SprintSpeed;
		GetCharacterMovement()->MaxAcceleration = PlayerData->SprintSpeed * PlayerData->AccelerationModifier;
	}
	else
	{
		PlayerData->bIsSprinting = false;
		StaminaComponent->SetIsSprinting(false);
		GetCharacterMovement()->MaxWalkSpeed = PlayerData->WalkSpeed;
		GetCharacterMovement()->MaxAcceleration = PlayerData->WalkSpeed * PlayerData->AccelerationModifier;
	}
}