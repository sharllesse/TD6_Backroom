// Copyright (c) Logicraft Interactive. All Rights Reserved.

#include "AI/Tasks/BTTask_Wander.h"

UBTTask_Wander::UBTTask_Wander(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "Wander";
}

void UBTTask_Wander::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	
	EQSRequest.InitForOwnerAndBlackboard(*this, GetBlackboardAsset());
	MoveToRequest.InitForOwnerAndBlackboard(*this, GetBlackboardAsset());
}

uint16 UBTTask_Wander::GetInstanceMemorySize() const
{
	return sizeof(FBTWanderTaskMemory);
}

EBTNodeResult::Type UBTTask_Wander::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (UAITask_Wander* WanderAiTask = UAITask_Base::Create<UAITask_Wander>(*OwnerComp.GetAIOwner(), *this))
	{
		WanderAiTask->EQSRequest = &EQSRequest;
		WanderAiTask->MoveToRequest = &MoveToRequest;

		WanderAiTask->Activate();

		FBTWanderTaskMemory* TaskMemory{ CastNodeMemory(NodeMemory) };
		TaskMemory->WanderTask = WanderAiTask;
		TaskMemory->bIsInitialized = true;

		return EBTNodeResult::InProgress;
	}
	
	return EBTNodeResult::Failed;
}

void UBTTask_Wander::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
	const UAITask_Wander* WanderTask{ Cast<UAITask_Wander>(&Task) };
	UBehaviorTreeComponent* BehaviorTreeComponent{ GetBTComponentForTask(Task) };
	
	FinishLatentTask(*BehaviorTreeComponent,
					 WanderTask->HasFailed() ? EBTNodeResult::Failed : EBTNodeResult::Succeeded);
}

EBTNodeResult::Type UBTTask_Wander::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FBTWanderTaskMemory* TaskMemory{ CastNodeMemory(NodeMemory) };

	if (TaskMemory->WanderTask.IsValid())
	{
		TaskMemory->WanderTask->FinishFailedTask();	
	}
	
	return EBTNodeResult::Aborted;
}

void UBTTask_Wander::CleanupMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTMemoryClear::Type CleanupType) const
{
	CleanupNodeMemory<FBTWanderTaskMemory>(NodeMemory, CleanupType);
}

void UBTTask_Wander::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
                                    EBTNodeResult::Type TaskResult)
{
	FBTWanderTaskMemory* WanderTaskMemory{ CastNodeMemory(NodeMemory) };
	WanderTaskMemory->Clean();
}
