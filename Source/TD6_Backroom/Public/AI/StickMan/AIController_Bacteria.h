// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/AIController_Base.h"
#include "BehaviorTree/Blackboard/BlackboardKey.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "AIController_Bacteria.generated.h"

class AAICharacter_Bacteria;

UCLASS()
class TD6_BACKROOM_API AAIController_Bacteria : public AAIController_Base
{
	GENERATED_BODY()

	TWeakObjectPtr<AAICharacter_Bacteria> Character_Bacteria;
	
public:
	AAIController_Bacteria();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void OnSetupBlackboardKey(AAICharacter_Base* InPawn, UBlackboardComponent* BlackboardComponent) override;
	
	virtual void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus) override;

private:
	EBlackboardNotificationResult OnSprintingStateChanged(const UBlackboardComponent&, FBlackboard::FKey KeyID);
};
