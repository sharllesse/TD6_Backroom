// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GenericTeamAgentInterface.h"
#include "AI/AICharacter_Base.h"
#include "AICharacter_Bacteria.generated.h"

UCLASS()
class TD6_BACKROOM_API AAICharacter_Bacteria : public AAICharacter_Base, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

protected:
	TWeakObjectPtr<AAIController> AIController;
	
public:
	AAICharacter_Bacteria();

	virtual void SetGenericTeamId(const FGenericTeamId& TeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;

	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

protected:
	virtual void BeginPlay() override;
};
