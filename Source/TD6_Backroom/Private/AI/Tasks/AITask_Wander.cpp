// Copyright (c) Logicraft Interactive. All Rights Reserved.

#include "AI/Tasks/AITask_Wander.h"

#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"

void UAITask_Wander::Activate()
{
	Super::Activate();

	if (!EQSRequest->IsValid() || !MoveToRequest->IsValid())
	{
		FinishFailedTask();
		return;
	}
	
	AAIController* AiController{ GetAIController() };
	UBlackboardComponent* BlackboardComponent{ AiController->GetBlackboardComponent() };

	MoveToRequest->SetRequestFinishedCallback(AiController, this, &UAITask_Wander::OnMoveRequestFinished);
	
	QueryFinishedSignature = FQueryFinishedSignature::CreateUObject(this, &UAITask_Wander::OnQueryRequestFinished);
	CachedQueryID = EQSRequest->Execute(*GetAIController(), BlackboardComponent, QueryFinishedSignature);
}

void UAITask_Wander::Clean()
{
	UEnvQueryManager* EQSManager = UEnvQueryManager::GetCurrent(GetWorld());
	check(EQSManager)
	
	EQSManager->AbortQuery(CachedQueryID);

	AAIController* AiController{ GetAIController() };
	MoveToRequest->UnSetRequestFinishedCallback(AiController, this);
	MoveToRequest->AbortMove(AiController, *this, FPathFollowingResultFlags::OwnerFinished, CachedMoveID);	
}

void UAITask_Wander::OnQueryRequestFinished(TSharedPtr<FEnvQueryResult> QueryResult)
{
	if (!QueryResult.IsValid())
	{
		FinishFailedTask();
		return;
	}
	
	AAIController* AiController{ GetAIController() };
	TVariant<AActor*, FVector> GoalActor;
	EPathFollowingRequestResult::Type PathFollowRequestResult{};

	if (QueryResult->Items.IsEmpty())
	{
		FinishFailedTask();
		return;
	}
	
	if (QueryResult->ItemType == UEnvQueryItemType_Actor::StaticClass())
	{
		GoalActor.Set<AActor*>(QueryResult->GetItemAsActor(0));
		PathFollowRequestResult = MoveToRequest->Execute(AiController, GoalActor, CachedMoveID);
	}
	else
	{
		GoalActor.Set<FVector>(QueryResult->GetItemAsLocation(0));
		PathFollowRequestResult = MoveToRequest->Execute(AiController, GoalActor, CachedMoveID);	
	}

	if (PathFollowRequestResult == EPathFollowingRequestResult::AlreadyAtGoal)
	{
		FinishTask();
	}

	if (PathFollowRequestResult == EPathFollowingRequestResult::Failed)
	{
		FinishFailedTask();
	}
}

void UAITask_Wander::OnMoveRequestFinished(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	if (RequestID != CachedMoveID)
	{
		return;
	}

	if (Result.IsSuccess())
	{
		FinishTask();
		return;
	}

	FinishFailedTask();
}
