// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TimerHolder.h"
#include "AI/AICharacter_Base.h"
#include "AICharacter_Bacteria.generated.h"

UCLASS()
class UBRBacteriaData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed{250.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed{500.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float AccelerationModifier{8.f};

	UPROPERTY(BlueprintReadWrite, Category = "Movement")
	bool bIsSprinting{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	TObjectPtr<USoundCue> RandomSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	TObjectPtr<USoundCue> ChaseScream;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hearing")
	float SprintThreshold{ 0.5f };
};

class AAIController_Bacteria;

UCLASS()
class TD6_BACKROOM_API AAICharacter_Bacteria : public AAICharacter_Base
{
	GENERATED_BODY()

	friend AAIController_Bacteria;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UBRBacteriaData> BacteriaData;

	float MaxVHSpeed{ 0.f };

	FDelegateHandle Handle;

	FTimerHolder RandomSoundTimer;
	
public:
	AAICharacter_Bacteria();

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void UpdateSprintState(bool bShouldSprint);

	UFUNCTION(NetMulticast, Reliable)
	void MakeRandomNoise();
};
