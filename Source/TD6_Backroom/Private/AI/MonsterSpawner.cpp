// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MonsterSpawner.h"

#include "Chain.h"
#include "Engine/TargetPoint.h"
#include "GameInstance/BRGameInstance.h"


// Sets default values
AMonsterSpawner::AMonsterSpawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMonsterSpawner::BeginPlay()
{
	Super::BeginPlay();

	
	if (HasAuthority())
	{
		TArray<AActor*> SpawnPoint;
		GetAttachedActors(SpawnPoint);
		checkf(!SpawnPoint.IsEmpty(), TEXT("Monster spawner has no spawn point in his child"));
		Chain::Execute(GetGameInstance<UBRGameInstance>(), [this, SpawnPoint](UBRGameInstance* GameInstance)
		{
			for (int i = 0; i < GameInstance->GetMonsterNumber(); ++i)
			{				
				int SpawnIndex = FMath::RandRange(0, SpawnPoint.Num() - 1);
				FTransform SpawnTransform = SpawnPoint[SpawnIndex]->GetActorTransform();
				
				UE_LOG(LogTemp, Error, TEXT("Spawning a monster at the spawn point %s"), *SpawnPoint[SpawnIndex]->GetName());
				
				GetWorld()->SpawnActor<ACharacter>(MonsterToSpawn, SpawnTransform);
			}
		});
	}
	
}

// Called every frame
void AMonsterSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

