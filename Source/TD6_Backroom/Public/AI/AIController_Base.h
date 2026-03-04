// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "AIController_Base.generated.h"

class AAICharacter_Base;

UCLASS()
class TD6_BACKROOM_API AAIController_Base : public AAIController
{
	GENERATED_BODY()

	friend AAICharacter_Base;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UBehaviorTree> AssignedBehaviorTree;

	UPROPERTY()
	TObjectPtr<UAISenseConfig_Sight> SenseConfig_Sight;
	UPROPERTY()
	TObjectPtr<UAISenseConfig_Hearing> SenseConfig_Hearing;

	TWeakObjectPtr<AAICharacter_Base> Character_Base;
public:
	AAIController_Base();

protected:
	virtual void OnSetupBlackboardKey(AAICharacter_Base* InPawn, UBlackboardComponent* BlackboardComponent) {}
	
	virtual void OnPossess(APawn* InPawn) override;
	
	UFUNCTION()
	virtual void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus) {}
};
