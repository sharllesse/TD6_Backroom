// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AIController_Base.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/AIPerceptionComponent.h"

AAIController_Base::AAIController_Base()
{
	PrimaryActorTick.bCanEverTick = true;

	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("Perception Component");

	SenseConfig_Sight = CreateDefaultSubobject<UAISenseConfig_Sight>("Sight Sense");
	PerceptionComponent->ConfigureSense(*SenseConfig_Sight);
}

void AAIController_Base::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	UBlackboardComponent* BlackboardComp;
	if (UseBlackboard(AssignedBehaviorTree->BlackboardAsset, BlackboardComp))
	{
		OnSetupBlackboardKey(BlackboardComp);
		
		RunBehaviorTree(AssignedBehaviorTree);
	}

	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AAIController_Base::OnTargetPerceptionUpdated);
}
