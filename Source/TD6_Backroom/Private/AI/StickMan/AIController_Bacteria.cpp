// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/StickMan/AIController_Bacteria.h"
#include "AI/StickMan/AICharacter_Bacteria.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Character/BRPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/PlayingSoundSubsystem.h"
#include "Sound/SoundCue.h"

AAIController_Bacteria::AAIController_Bacteria()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAIController_Bacteria::BeginPlay()
{
	Super::BeginPlay();
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

	BlackboardComponent->RegisterObserver(
		BlackboardComponent->GetKeyID("TargetActor"),
		this,
		FOnBlackboardChangeNotification::CreateUObject(this, &AAIController_Bacteria::OnTargetActorChanged));
}

void AAIController_Bacteria::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (Actor->ActorHasTag(ABRPlayerCharacter::Dead))
	{
		return;
	}
	
	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			if (Blackboard->GetValueAsObject(TEXT("TargetActor")))
			{
				return;
			}
			
			Blackboard->ClearValue(TEXT("LostLocation"));
			Blackboard->SetValueAsObject(TEXT("TargetActor"), Actor);
			
			return;
		}

		Blackboard->ClearValue(TEXT("TargetActor"));
		Blackboard->SetValueAsVector(TEXT("LostLocation"), Stimulus.StimulusLocation);
	}

	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
	{
		if (Blackboard->GetValueAsObject(TEXT("TargetActor")))
		{
			return;
		}

		if (!IsValid(GetPawn()))
		{
			return;
		}
			
		const float Distance = FVector::Dist(GetPawn()->GetActorLocation(), Stimulus.StimulusLocation);

		const float DistanceFactor = FMath::GetMappedRangeValueClamped(
					FVector2D(0.0f, SenseConfig_Hearing->HearingRange),
					FVector2D(1.0f, 0.0f),
					Distance
				);

		const float PerceivedLoudness = Stimulus.Strength * DistanceFactor;

		UE_LOG(LogTemp, Warning, TEXT("%f"), PerceivedLoudness)
		
		if (PerceivedLoudness > Character_Bacteria->BacteriaData->SprintThreshold)
		{
			Blackboard->SetValueAsBool(TEXT("IsSprinting"), true);
		}
		
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

	UE_LOG(LogTemp, Error, TEXT("Team id %d, Other team %d"), GetGenericTeamId().GetId(), OtherTeam)
	
	return OtherTeam == GetGenericTeamId() ? ETeamAttitude::Friendly : ETeamAttitude::Hostile;
}

EBlackboardNotificationResult AAIController_Bacteria::OnSprintingStateChanged(const UBlackboardComponent&,
                                                                              FBlackboard::FKey KeyID)
{
	const bool bIsSprinting{ Blackboard->GetValue<UBlackboardKeyType_Bool>(KeyID) };
	
	Character_Bacteria->UpdateSprintState(bIsSprinting);
	
	return EBlackboardNotificationResult::ContinueObserving;
}

EBlackboardNotificationResult AAIController_Bacteria::OnTargetActorChanged(const UBlackboardComponent&,
	FBlackboard::FKey KeyID)
{
	bool bIsValid{ IsValid(Blackboard->GetValue<UBlackboardKeyType_Object>(KeyID)) };
	
	if (bIsValid)
	{
		MakeScreamNoise();
		
		return EBlackboardNotificationResult::ContinueObserving;
	}

	StopScreamNoise();
	
	return EBlackboardNotificationResult::ContinueObserving;
}

void AAIController_Bacteria::MakeScreamNoise_Implementation()
{
	if (StopChaseSoundTimer.IsAlreadyRunning())
	{
		StopChaseSoundTimer.Clear();
		return;
	}
	UE_LOG(LogTemp, Error, TEXT("%p"), Character_Bacteria->BacteriaData->ChaseScream.Get());
	UPlayingSoundSubsystem::Get(this)->PlaySoundAttached(
		Character_Bacteria->BacteriaData->ChaseScream, Character_Bacteria->GetMesh());
}

void AAIController_Bacteria::StopScreamNoise_Implementation()
{
	StopChaseSoundTimer.Schedule([this]
	{
		UPlayingSoundSubsystem::Get(this)->StopSound(Character_Bacteria->BacteriaData->ChaseScream);
	}, { false, 2.5f });
}
