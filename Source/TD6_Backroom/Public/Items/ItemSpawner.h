// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemSpawner.generated.h"

class APickableItem;

UCLASS()
class TD6_BACKROOM_API AItemSpawner : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AItemSpawner();

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APickableItem> ItemToSpawn;
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	FDelegateHandle SpawnVHSHandle;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
