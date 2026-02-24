// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "AIController_Base.generated.h"

UCLASS()
class TD6_BACKROOM_API AAIController_Base : public AAIController
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UBehaviorTree> AssignedBehaviorTree;

	TWeakObjectPtr<UAISenseConfig_Sight> SenseConfig_Sight;
public:
	AAIController_Base();

protected:
	virtual void OnSetupBlackboardKey(UBlackboardComponent* BlackboardComponent) {}
	
	virtual void OnPossess(APawn* InPawn) override;
	
	UFUNCTION()
	virtual void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus) {}
};
