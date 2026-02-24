// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_ClearValue.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ClearValue::UBTTask_ClearValue()
{
	NodeName = "Clear Blackboard Value";
}

EBTNodeResult::Type UBTTask_ClearValue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent())
	{
		BlackboardComponent->ClearValue(GetSelectedBlackboardKey());

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
