// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/StickMan/AIController_StickMan.h"


// Sets default values
AAIController_StickMan::AAIController_StickMan()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AAIController_StickMan::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAIController_StickMan::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

