// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/StickMan/AICharacter_Bacteria.h"

#include "EventBus.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameState/BRGameGameState.h"
#include "Items/BRItemGameplayTag.h"
#include "Items/ItemData.h"

AAICharacter_Bacteria::AAICharacter_Bacteria()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAICharacter_Bacteria::BeginPlay()
{
	Super::BeginPlay();
	
	GetCharacterMovement()->MaxWalkSpeed = BacteriaData->WalkSpeed;
	GetCharacterMovement()->MaxAcceleration = BacteriaData->WalkSpeed * BacteriaData->AccelerationModifier;

	Handle = UEventBus::AddLambda(this, Item_Callback_OnItemPickUp,
	[this](const FItemData& ItemData)
	{
		if (ItemData.Type != Item_VHS)
		{
			return;
		}

		if (auto* GameState = GetWorld()->GetGameState<ABRGameGameState>())
		{
			if (ItemData.Count / GameState->VhsToCollect < 0.5f)
			{
				return;
			}

			MaxVHSpeed += BacteriaData->SprintSpeed / GameState->VhsToCollect;
			// MaxVHSpeed = 25.f * (ItemData.Count / GameState->VhsToCollect * 10.f);
		}
	});
}

void AAICharacter_Bacteria::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UEventBus::Remove(this, Item_Callback_OnItemPickUp, Handle);
}

void AAICharacter_Bacteria::UpdateSprintState(bool bShouldSprint)
{
	if (bShouldSprint)
	{
		BacteriaData->bIsSprinting = true;
		GetCharacterMovement()->MaxWalkSpeed = BacteriaData->SprintSpeed + MaxVHSpeed;
		GetCharacterMovement()->MaxAcceleration = (BacteriaData->SprintSpeed + MaxVHSpeed) * BacteriaData->AccelerationModifier;
	}
	else
	{
		BacteriaData->bIsSprinting = false;
		GetCharacterMovement()->MaxWalkSpeed = BacteriaData->WalkSpeed;
		GetCharacterMovement()->MaxAcceleration = BacteriaData->WalkSpeed * BacteriaData->AccelerationModifier;
	}
}
