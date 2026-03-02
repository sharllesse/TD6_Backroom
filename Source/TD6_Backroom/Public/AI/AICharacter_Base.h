// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Character.h"
#include "AICharacter_Base.generated.h"

UCLASS()
class TD6_BACKROOM_API AAICharacter_Base : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

	friend class AAIController_Base;
protected:
	TWeakObjectPtr<AAIController_Base> AIController_Base;
	
public:
	AAICharacter_Base();

	virtual void SetGenericTeamId(const FGenericTeamId& TeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;

	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	
protected:
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;
};
