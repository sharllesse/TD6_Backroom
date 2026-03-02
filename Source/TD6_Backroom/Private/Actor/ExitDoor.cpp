// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/ExitDoor.h"

#include "EventBus.h"
#include "GameState/BRGameStateGameplayTags.h"


// Sets default values
AExitDoor::AExitDoor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>("Door mesh");
	DoorMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void AExitDoor::BeginPlay()
{
	Super::BeginPlay();
	OnObjectiveCompletedHandle = UEventBus::AddLambda(this, GameState_Callback_OnObjectivesCompleted, [this]
	{
		MakeRotation();
	});
}

void AExitDoor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UEventBus::Remove(this, GameState_Callback_OnObjectivesCompleted, OnObjectiveCompletedHandle);
}

void AExitDoor::MakeRotation_Implementation()
{
	UE_LOG(LogTemp, Error, TEXT("Need MakeRotation blueprint implementation"));
}

// Called every frame
void AExitDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

