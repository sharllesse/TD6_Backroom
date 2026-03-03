// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/StaminaComponent.h"


// Sets default values for this component's properties
UStaminaComponent::UStaminaComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	// ...
}


// Called when the game starts
void UStaminaComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentStamina = MaxStamina;
	// ...
	
}


// Called every frame
void UStaminaComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                      FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!bIsSprinting)
	{
		CurrentStamina = FMath::Min(MaxStamina, CurrentStamina += StaminaRegenSpeed * GetWorld()->DeltaTimeSeconds);
		OnStaminaChange.ExecuteIfBound(CurrentStamina, MaxStamina);
		if (FMath::IsNearlyEqual(CurrentStamina, 100.f))
		{
			SetComponentTickEnabled(false);
		}
	}
	else
	{
		CurrentStamina = FMath::Max(0, CurrentStamina -= StaminaDrainSpeed * GetWorld()->DeltaTimeSeconds);
		OnStaminaChange.ExecuteIfBound(CurrentStamina, MaxStamina);
		if (FMath::IsNearlyZero(CurrentStamina))
		{
			OnCantSprint.ExecuteIfBound();
			SetComponentTickEnabled(false);
			StaminaRegenTimer.Schedule([this]
			{
				SetComponentTickEnabled(true);
			},{.bIsLooping = false, .Rate = 0.001,.FirstDelay = StaminaRegenStart});
		}
	}
}

bool UStaminaComponent::CanSprint() const
{
	return !FMath::IsNearlyZero(CurrentStamina);
}

void UStaminaComponent::SetIsSprinting(bool bInIsSprinting)
{
	bIsSprinting = bInIsSprinting;
	SetComponentTickEnabled(bInIsSprinting);
	StaminaRegenTimer.Schedule([this]
	{
		SetComponentTickEnabled(true);
	},{.bIsLooping = false, .Rate = 0.001,.FirstDelay = StaminaRegenStart});
	
}
