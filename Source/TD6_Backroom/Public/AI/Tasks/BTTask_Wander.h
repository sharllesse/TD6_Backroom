// Copyright (c) Logicraft Interactive. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AI/Tasks/AITask_Wander.h"
#include "AI/Tasks/OWAiType.h"
#include "BehaviorTree/BTTaskNode.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "BTTask_Wander.generated.h"

struct FBTWanderTaskMemory : FBTBaseTaskMemory
{
	TWeakObjectPtr<UAITask_Wander> WanderTask;
	
	void Clean()
	{
		bIsInitialized = false;
		WanderTask.Reset();
	}
};

/**
 * 
 */
UCLASS()
class TD6_BACKROOM_API UBTTask_Wander : public UBTTaskNode, public TNodeMemoryHelper<FBTWanderTaskMemory>
{
	GENERATED_BODY()

protected:
	UPROPERTY(Category = EQS, EditAnywhere)
	FEQSParametrizedQueryExecutionRequest EQSRequest;

	UPROPERTY(Category = Movement, EditAnywhere)
	FMoveToParametrizedExecutionRequest MoveToRequest;
public:
	UBTTask_Wander(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

	virtual uint16 GetInstanceMemorySize() const override;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void OnGameplayTaskDeactivated(UGameplayTask& Task) override;

	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
};
