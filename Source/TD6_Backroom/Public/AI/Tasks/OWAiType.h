// Copyright (c) Logicraft Interactive. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/ValueOrBBKey.h"
#include "NavFilters/NavigationQueryFilter.h"
#include "Navigation/PathFollowingComponent.h"
#include "OWAiType.generated.h"

struct TD6_BACKROOM_API FBTBaseTaskMemory
{
	bool bIsInitialized{ false };
};

/**
 * A helper class that can be inherited my behavior tree service, decorator and task.
 * It exists to avoid creating a cast node memory function.
 */
template<typename TNodeMemory>
class TD6_BACKROOM_API TNodeMemoryHelper
{
protected:
	TNodeMemory* CastNodeMemory(uint8* NodeMemory) const
	{
		return reinterpret_cast<TNodeMemory*>(NodeMemory);
	}
	
	TNodeMemory* CastNodeMemory(const UBlackboardComponent* BlackboardComponent) const
	{
		UBehaviorTreeComponent* BehaviorTreeComponent{ Cast<UBehaviorTreeComponent>(BlackboardComponent->GetBrainComponent()) };
		uint8* NodeMemory{ BehaviorTreeComponent->GetNodeMemory(this, BehaviorTreeComponent->FindInstanceContainingNode(this)) };
		return reinterpret_cast<TNodeMemory*>(NodeMemory);
	}

	/**
	 * @brief This function is meant to be used with FValueOrBlackboardKey (ex: FValueOrBBKey_Bool).
	 * Since it could maybe contain a blackboard value or not a callback could be linked to it.
	 */
	template<typename TValueOrBlackboardKey, typename TUserObject, typename TMemberFunction>
		requires
			std::derived_from<TValueOrBlackboardKey, FValueOrBlackboardKeyBase> &&
			std::derived_from<TUserObject, UObject> &&
			std::is_member_function_pointer_v<TMemberFunction>
	void RegisterToKeyIdChecked(TValueOrBlackboardKey& ValueOrBlackboardKeyBase,
	UBehaviorTreeComponent& BehaviorTreeComponent, UBlackboardComponent* BlackboardComponent, TUserObject* UserObject, TMemberFunction MemberFunction)
	{
		const FBlackboard::FKey KeyId{ ValueOrBlackboardKeyBase.GetKeyId(BehaviorTreeComponent) };
		if (KeyId != FBlackboard::InvalidKey)
		{
			BlackboardComponent->RegisterObserver(KeyId, UserObject,
				FOnBlackboardChangeNotification::CreateUObject(UserObject, MemberFunction));
		}
	}
};

/**
 * 
 */
USTRUCT()
struct TD6_BACKROOM_API FMoveToParametrizedExecutionRequest
{
	GENERATED_BODY()
	
	/** fixed distance added to threshold between AI and goal location in destination reach test */
	UPROPERTY(config, EditAnywhere, meta=(ClampMin = "0.0", UIMin="0.0"))
	FValueOrBBKey_Float AcceptableRadius;

	/** "None" will result in default filter being used */
	UPROPERTY(EditAnywhere)
	FValueOrBBKey_Class FilterClass = TSubclassOf<UNavigationQueryFilter>();

	/** if task is expected to react to changes to location represented by BB key 
	 *	this property can be used to tweak sensitivity of the mechanism. Value is 
	 *	recommended to be less than AcceptableRadius */
	UPROPERTY(EditAnywhere, meta = (ClampMin = "1", UIMin = "1"))
	FValueOrBBKey_Float ObservedBlackboardValueTolerance;

	UPROPERTY(EditAnywhere, DisplayName = AllowStrafe)
	FValueOrBBKey_Bool bAllowStrafe;

	/** if set, radius of AI's capsule will be added to threshold between AI and goal location in destination reach test  */
	UPROPERTY(EditAnywhere, DisplayName = ReachTestIncludesAgentRadius)
	FValueOrBBKey_Bool bReachTestIncludesAgentRadius;
	
	/** if set, radius of goal's capsule will be added to threshold between AI and goal location in destination reach test  */
	UPROPERTY(EditAnywhere, DisplayName = ReachTestIncludesGoalRadius)
	FValueOrBBKey_Bool bReachTestIncludesGoalRadius;

	/** if set, the path request will start from the end of the previous path (if any), and the generated path will be merged with the remaining points of the previous path */
	UPROPERTY(EditAnywhere, DisplayName = StartFromPreviousPath)
	FValueOrBBKey_Bool bStartFromPreviousPath;

	/** if set, use incomplete path when goal can't be reached */
	UPROPERTY(EditAnywhere, DisplayName = AllowPartialPath)
	FValueOrBBKey_Bool bAllowPartialPath;

	/** if set, path to goal actor will update itself when actor moves */
	UPROPERTY(EditAnywhere, DisplayName = TrackMovingGoal)
	FValueOrBBKey_Bool bTrackMovingGoal;

	/** if set, the goal location will need to be navigable */
	UPROPERTY(EditAnywhere, DisplayName = RequireNavigableEndLocation)
	FValueOrBBKey_Bool bRequireNavigableEndLocation;

	/** if set, goal location will be projected on navigation data (navmesh) before using */
	UPROPERTY(EditAnywhere, DisplayName = ProjectGoalLocation)
	FValueOrBBKey_Bool bProjectGoalLocation;

private:
	bool bInitialized{ false };
	
public:
	
	FMoveToParametrizedExecutionRequest();

	void InitForOwnerAndBlackboard(UObject& Owner, UBlackboardData* BBAsset);

	template<typename TClass>
	void SetRequestFinishedCallback(AAIController* AIController, TClass* UserObject, TMemFunPtrType<false, TClass, void(FAIRequestID /*RequestID*/, const FPathFollowingResult& /*Result*/)>::Type InMemberFunction)
	{
		AIController->GetPathFollowingComponent()->OnRequestFinished.AddUObject(UserObject, InMemberFunction);
	}

	template<typename TClass>
	void SetRequestFinishedCallback(AAIController* AIController, const TClass* UserObject, TMemFunPtrType<true, TClass, void(FAIRequestID /*RequestID*/, const FPathFollowingResult& /*Result*/)>::Type InMemberFunction)
	{
		AIController->GetPathFollowingComponent()->OnRequestFinished.AddUObject(UserObject, InMemberFunction);
	}

	void UnSetRequestFinishedCallback(AAIController* AIController, UObject* UserObject);

	EPathFollowingRequestResult::Type Execute(AAIController* AIController, TVariant<AActor*, FVector> Goal, FAIRequestID& AIRequestID);
	
	void AbortMove(AAIController* AIController, const UObject& Instigator, FPathFollowingResultFlags::Type AbortFlags, FAIRequestID RequestID);
	
	bool IsValid() const { return bInitialized; }
};
