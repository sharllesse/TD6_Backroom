// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "AIController_Base.generated.h"

UCLASS()
class TD6_BACKROOM_API AAIController_Base : public AAIController
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAIController_Base();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
