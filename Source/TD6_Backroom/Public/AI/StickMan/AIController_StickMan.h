// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/AIController_Base.h"
#include "AIController_StickMan.generated.h"

UCLASS()
class TD6_BACKROOM_API AAIController_StickMan : public AAIController_Base
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAIController_StickMan();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
