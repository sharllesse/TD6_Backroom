// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/StickMan/AICharacter_StickMan.h"


// Sets default values
AAICharacter_StickMan::AAICharacter_StickMan()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AAICharacter_StickMan::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAICharacter_StickMan::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AAICharacter_StickMan::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

