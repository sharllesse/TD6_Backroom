// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ItemSpawner.h"

#include "Chain.h"
#include "GameInstance/BRGameInstance.h"
#include "GameMode/BRGameGameMode.h"
#include "GameMode/BRGameModeGameplayTags.h"
#include "GameState/BRGameGameState.h"
#include "Items/PickableItem.h"


// Sets default values
AItemSpawner::AItemSpawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AItemSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnVHSHandle = UEventBus::AddLambda(this, GameMode_Callback_OnBeginPlayFinish, [this]
	{
		if (HasAuthority())
		{
			TArray<AActor*> SpawnPoint;
			GetAttachedActors(SpawnPoint);
			Chain::Execute(GetWorld()->GetAuthGameMode<ABRGameGameMode>(), [this, &SpawnPoint](ABRGameGameMode* GameMode)
			{
				UE_LOG(LogTemp, Error, TEXT("Start spawning %d VHS"), GameMode->GetVhsObjective());
				for (int i = 0; i < GameMode->GetVhsObjective(); ++i)
				{
					checkf(!SpawnPoint.IsEmpty(), TEXT("Not enough spawn point for this item spawner"));
				
					int SpawnIndex = FMath::RandRange(0, SpawnPoint.Num() - 1);
					FTransform SpawnTransform = SpawnPoint[SpawnIndex]->GetActorTransform();
				
					UE_LOG(LogTemp, Error, TEXT("Spawning a item at the spawn point %s"), *SpawnPoint[SpawnIndex]->GetName());
				
					GetWorld()->SpawnActor<APickableItem>(ItemToSpawn, SpawnTransform);
					SpawnPoint.RemoveAt(SpawnIndex);
				}
				UE_LOG(LogTemp, Error, TEXT("End spawning"));
			});
		}
	});
}

void AItemSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UEventBus::Remove(this, GameMode_Callback_OnBeginPlayFinish, SpawnVHSHandle);
}

// Called every frame
void AItemSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

