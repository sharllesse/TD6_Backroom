// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MonsterSpawner.h"

#include "Chain.h"
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
		Chain::Execute(GetGameInstance<UBRGameInstance>(), [this](UBRGameInstance* GameInstance)
		{
			for (int i = 0; i < GameInstance->GetMonsterNumber(); ++i)
			{
				GetWorld()->SpawnActor<ACharacter>(MonsterToSpawn, GetActorLocation(), GetActorRotation());
			}
		});
	}
	
}

// Called every frame
void AMonsterSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

