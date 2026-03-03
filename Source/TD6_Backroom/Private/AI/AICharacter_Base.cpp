// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AICharacter_Base.h"
#include "AI/AIController_Base.h"

AAICharacter_Base::AAICharacter_Base()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAICharacter_Base::SetGenericTeamId(const FGenericTeamId& TeamID)
{
	AIController_Base->SetGenericTeamId(TeamID);
}

FGenericTeamId AAICharacter_Base::GetGenericTeamId() const
{
	return AIController_Base->GetGenericTeamId();
}

ETeamAttitude::Type AAICharacter_Base::GetTeamAttitudeTowards(const AActor& Other) const
{
	const IGenericTeamAgentInterface* GenericTeamAgentInterface{ Cast<const IGenericTeamAgentInterface>(&Other) };
	if (GenericTeamAgentInterface->GetGenericTeamId() == 2)
	{
		return ETeamAttitude::Neutral;
	}
	
	return ETeamAttitude::Hostile;
}

void AAICharacter_Base::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAICharacter_Base::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AIController_Base = Cast<AAIController_Base>(NewController);
}
