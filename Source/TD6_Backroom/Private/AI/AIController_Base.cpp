// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AIController_Base.h"


// Sets default values
AAIController_Base::AAIController_Base()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AAIController_Base::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAIController_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

