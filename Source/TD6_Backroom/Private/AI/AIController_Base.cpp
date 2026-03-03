// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AIController_Base.h"
#include "BehaviorTree/BehaviorTree.h"
#include "AI/AICharacter_Base.h"
#include "Perception/AIPerceptionComponent.h"

AAIController_Base::AAIController_Base()
{
	PrimaryActorTick.bCanEverTick = true;

	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("Perception Component");

	SenseConfig_Sight = CreateDefaultSubobject<UAISenseConfig_Sight>("Sight Sense");
	SenseConfig_Sight->DetectionByAffiliation.bDetectEnemies = true;
	SenseConfig_Sight->DetectionByAffiliation.bDetectFriendlies = false;
	SenseConfig_Sight->DetectionByAffiliation.bDetectNeutrals = false;
	
	PerceptionComponent->ConfigureSense(*SenseConfig_Sight);

	SenseConfig_Hearing = CreateDefaultSubobject<UAISenseConfig_Hearing>("Hearing Sense");

	PerceptionComponent->ConfigureSense(*SenseConfig_Hearing);

	AAIController::SetGenericTeamId(1);
}

void AAIController_Base::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	Character_Base = Cast<AAICharacter_Base>(InPawn);
	
	UBlackboardComponent* BlackboardComp;
	if (UseBlackboard(AssignedBehaviorTree->BlackboardAsset, BlackboardComp))
	{
		OnSetupBlackboardKey(Character_Base.Get(), BlackboardComp);
		
		RunBehaviorTree(AssignedBehaviorTree);
	}

	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AAIController_Base::OnTargetPerceptionUpdated);
}
