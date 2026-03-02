// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/StickMan/AIController_Bacteria.h"
#include "AI/StickMan/AICharacter_Bacteria.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"

AAIController_Bacteria::AAIController_Bacteria()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAIController_Bacteria::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	Blackboard->UnregisterObserversFrom(this);	
}

void AAIController_Bacteria::OnSetupBlackboardKey(AAICharacter_Base* InPawn, UBlackboardComponent* BlackboardComponent)
{
	Character_Bacteria = Cast<AAICharacter_Bacteria>(InPawn);
	
	BlackboardComponent->RegisterObserver(
		BlackboardComponent->GetKeyID("IsSprinting"),
		this,
		FOnBlackboardChangeNotification::CreateUObject(this, &AAIController_Bacteria::OnSprintingStateChanged));
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

EBlackboardNotificationResult AAIController_Bacteria::OnSprintingStateChanged(const UBlackboardComponent&,
	FBlackboard::FKey KeyID)
{
	const bool bIsSprinting{ Blackboard->GetValue<UBlackboardKeyType_Bool>(KeyID) };
	
	Character_Bacteria->UpdateSprintState(bIsSprinting);
	
	return EBlackboardNotificationResult::ContinueObserving;
}
