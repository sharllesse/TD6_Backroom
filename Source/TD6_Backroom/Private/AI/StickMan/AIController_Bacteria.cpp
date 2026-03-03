// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/StickMan/AIController_Bacteria.h"

#include "EventBus.h"
#include "Linq.h"
#include "AI/StickMan/AICharacter_Bacteria.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "Character/BRPlayerCharacter.h"
#include "PlayerState/BRPlayerStateGameTags.h"

AAIController_Bacteria::AAIController_Bacteria()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAIController_Bacteria::BeginPlay()
{
	Super::BeginPlay();
	// OnPlayerDeath = UEventBus::AddLambda(this, PlayerState_Callback_Dies, [this]
	// {
	// 	AActor* CurrentTarget = Cast<AActor>(Blackboard->GetValueAsObject(TEXT("TargetActor")));
	// 	if (CurrentTarget && CurrentTarget->ActorHasTag(ABRPlayerCharacter::DeadTag))
	// 	{
	// 		Blackboard->ClearValue(TEXT("TargetActor"));
	// 		Blackboard->ClearValue(TEXT("LostLocation"));
	// 	}
	// });
}

void AAIController_Bacteria::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	Blackboard->UnregisterObserversFrom(this);	

	//UEventBus::Remove(this, PlayerState_Callback_Dies, OnPlayerDeath);
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
	if (Actor->ActorHasTag("Dead"))
	{
		return;
	}
	
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

ETeamAttitude::Type AAIController_Bacteria::GetTeamAttitudeTowards(const AActor& Other) const
{
	const IGenericTeamAgentInterface* GenericTeamAgentInterface{ Cast<const IGenericTeamAgentInterface>(&Other) };

	if (!GenericTeamAgentInterface)
	{
		return ETeamAttitude::Neutral;
	}

	const uint8 OtherTeam{ GenericTeamAgentInterface->GetGenericTeamId() };
	
	return OtherTeam == GetGenericTeamId() ? ETeamAttitude::Friendly : ETeamAttitude::Hostile;
}

EBlackboardNotificationResult AAIController_Bacteria::OnSprintingStateChanged(const UBlackboardComponent&,
                                                                              FBlackboard::FKey KeyID)
{
	const bool bIsSprinting{ Blackboard->GetValue<UBlackboardKeyType_Bool>(KeyID) };
	
	Character_Bacteria->UpdateSprintState(bIsSprinting);
	
	return EBlackboardNotificationResult::ContinueObserving;
}
