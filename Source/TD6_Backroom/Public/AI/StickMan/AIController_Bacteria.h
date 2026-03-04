// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TimerHolder.h"
#include "AI/AIController_Base.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BehaviorTree/Blackboard/BlackboardKey.h"
#include "AIController_Bacteria.generated.h"

class AAICharacter_Bacteria;

UCLASS()
class TD6_BACKROOM_API AAIController_Bacteria : public AAIController_Base
{
	GENERATED_BODY()

	TWeakObjectPtr<AAICharacter_Bacteria> Character_Bacteria;

	FTimerHolder StopChaseSoundTimer;
public:
	AAIController_Bacteria();

	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void OnSetupBlackboardKey(AAICharacter_Base* InPawn, UBlackboardComponent* BlackboardComponent) override;
	
	virtual void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus) override;

	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

private:
	EBlackboardNotificationResult OnSprintingStateChanged(const UBlackboardComponent&, FBlackboard::FKey KeyID);
	EBlackboardNotificationResult OnTargetActorChanged(const UBlackboardComponent&, FBlackboard::FKey KeyID);

	UFUNCTION(NetMulticast, Reliable)
	void MakeScreamNoise();

	UFUNCTION(NetMulticast, Reliable)
	void StopScreamNoise();
};
