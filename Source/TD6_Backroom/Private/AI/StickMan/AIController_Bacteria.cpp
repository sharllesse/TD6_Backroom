// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/StickMan/AIController_Bacteria.h"

#include "BehaviorTree/BlackboardComponent.h"

AAIController_Bacteria::AAIController_Bacteria()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAIController_Bacteria::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			Blackboard->ClearValue(TEXT("LostLocation"));
			Blackboard->SetValueAsObject(TEXT("TargetActor"), Actor);
			return;
		}
		
		Blackboard->ClearValue(TEXT("TargetActor"));
		Blackboard->SetValueAsVector(TEXT("LostLocation"), Stimulus.StimulusLocation);
	}
}
