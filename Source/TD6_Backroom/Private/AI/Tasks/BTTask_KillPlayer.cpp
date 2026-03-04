// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_KillPlayer.h"

#include "GenericTeamAgentInterface.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/BRPlayerCharacter.h"

UBTTask_KillPlayer::UBTTask_KillPlayer(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "Kill Player";

	PlayerToKill.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_KillPlayer, PlayerToKill), AActor::StaticClass());
}

void UBTTask_KillPlayer::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (const UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		PlayerToKill.ResolveSelectedKey(*BBAsset);
	}
	else
	{
		PlayerToKill.InvalidateResolvedKey();
	}
}

EBTNodeResult::Type UBTTask_KillPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComponent{ OwnerComp.GetBlackboardComponent() };
	UObject* PlayerObject = BlackboardComponent->GetValueAsObject(PlayerToKill.SelectedKeyName);
	
	if (ABRPlayerCharacter* PlayerCharacter{ Cast<ABRPlayerCharacter>(PlayerObject) })
	{
		BlackboardComponent->ClearValue(TEXT("TargetActor"));
		BlackboardComponent->ClearValue(TEXT("LostLocation"));

		PlayerCharacter->TriggerScreamer();
	}
	
	return EBTNodeResult::Succeeded;
}
