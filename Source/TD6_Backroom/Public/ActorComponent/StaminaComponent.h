// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TimerHolder.h"
#include "Components/ActorComponent.h"
#include "StaminaComponent.generated.h"

DECLARE_DELEGATE(OnCantSprintSignature)
DECLARE_DELEGATE_TwoParams(OnStaminaChangeSignature, float, float)

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TD6_BACKROOM_API UStaminaComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UStaminaComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Stamina")
	float MaxStamina{100.f};

	UPROPERTY(EditAnywhere, Category = "Stamina")
	float StaminaRegenSpeed{5.f};
	
	UPROPERTY(EditAnywhere, Category = "Stamina")
	float StaminaDrainSpeed{5.f};

	UPROPERTY(EditAnywhere, Category = "Stamina")
	float StaminaRegenStart{3.f};
	
	UPROPERTY(BlueprintReadWrite, Category = "Stamina")
	float CurrentStamina{0.f};

	bool bIsSprinting{false};

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	bool CanSprint() const;

	void SetIsSprinting(bool bInIsSprinting);

	OnCantSprintSignature OnCantSprint;
	OnStaminaChangeSignature OnStaminaChange;
};
