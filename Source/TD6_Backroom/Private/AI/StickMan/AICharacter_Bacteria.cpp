// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/StickMan/AICharacter_Bacteria.h"

#include "AIController.h"

AAICharacter_Bacteria::AAICharacter_Bacteria()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAICharacter_Bacteria::SetGenericTeamId(const FGenericTeamId& TeamID)
{
	AIController->SetGenericTeamId(TeamID);
}

FGenericTeamId AAICharacter_Bacteria::GetGenericTeamId() const
{
	return AIController->GetGenericTeamId();
}

ETeamAttitude::Type AAICharacter_Bacteria::GetTeamAttitudeTowards(const AActor& Other) const
{
	return ETeamAttitude::Hostile;
}

void AAICharacter_Bacteria::BeginPlay()
{
	Super::BeginPlay();

	AIController = Cast<AAIController>(Controller);
}
