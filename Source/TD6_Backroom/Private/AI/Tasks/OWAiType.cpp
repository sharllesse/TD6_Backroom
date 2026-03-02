// Copyright (c) Logicraft Interactive. All Rights Reserved.


#include "AI/Tasks/OWAiType.h"

FMoveToParametrizedExecutionRequest::FMoveToParametrizedExecutionRequest()
{
}

void FMoveToParametrizedExecutionRequest::InitForOwnerAndBlackboard(UObject& Owner, UBlackboardData* BBAsset)
{
	const float AcceptanceRadius = GET_AI_CONFIG_VAR(AcceptanceRadius);
	AcceptableRadius = AcceptanceRadius;
	ObservedBlackboardValueTolerance = AcceptanceRadius * 0.95f;
	bReachTestIncludesGoalRadius = bReachTestIncludesAgentRadius = GET_AI_CONFIG_VAR(bFinishMoveOnGoalOverlap);
	bAllowStrafe = GET_AI_CONFIG_VAR(bAllowStrafing);
	bAllowPartialPath = GET_AI_CONFIG_VAR(bAcceptPartialPaths);
	bTrackMovingGoal = true;
	bRequireNavigableEndLocation = true;
	bProjectGoalLocation = true;
	bStartFromPreviousPath = false;

	bInitialized = true;
}

void FMoveToParametrizedExecutionRequest::UnSetRequestFinishedCallback(AAIController* AIController, UObject* UserObject)
{
	AIController->GetPathFollowingComponent()->OnRequestFinished.RemoveAll(UserObject);
}

void FMoveToParametrizedExecutionRequest::AbortMove(AAIController* AIController, const UObject& Instigator, FPathFollowingResultFlags::Type AbortFlags, FAIRequestID RequestID)
{
	AIController->GetPathFollowingComponent()->AbortMove(Instigator, AbortFlags, RequestID);
}

EPathFollowingRequestResult::Type FMoveToParametrizedExecutionRequest::Execute(AAIController* AIController, TVariant<AActor*, FVector> Goal, FAIRequestID& AIRequestID)
{
	UBlackboardComponent* BlackboardComponent{ AIController->GetBlackboardComponent() };
	
	FAIMoveRequest MoveRequest{};
	if (const auto GoalActor = Goal.TryGet<AActor*>())
	{
		MoveRequest.SetGoalActor(*GoalActor);
	}
	else
	{
		MoveRequest.SetGoalLocation(Goal.Get<FVector>());
	}

	MoveRequest.SetAcceptanceRadius(AcceptableRadius.GetValue(BlackboardComponent));
	MoveRequest.SetAllowPartialPath(bAllowPartialPath.GetValue(BlackboardComponent));
	MoveRequest.SetCanStrafe(bAllowStrafe.GetValue(BlackboardComponent));
	MoveRequest.SetNavigationFilter(FilterClass.GetValue(BlackboardComponent));
	MoveRequest.SetProjectGoalLocation(bProjectGoalLocation.GetValue(BlackboardComponent));
	MoveRequest.SetReachTestIncludesAgentRadius(bReachTestIncludesAgentRadius.GetValue(BlackboardComponent));
	MoveRequest.SetReachTestIncludesGoalRadius(bReachTestIncludesGoalRadius.GetValue(BlackboardComponent));
	MoveRequest.SetRequireNavigableEndLocation(bRequireNavigableEndLocation.GetValue(BlackboardComponent));
	MoveRequest.SetStartFromPreviousPath(bStartFromPreviousPath.GetValue(BlackboardComponent));
	
	const FPathFollowingRequestResult PathResult{ AIController->MoveTo(MoveRequest) };
	AIRequestID = PathResult.MoveId;

	return PathResult.Code;
}
