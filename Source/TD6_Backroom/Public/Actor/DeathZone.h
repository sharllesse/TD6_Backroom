// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DeathZone.generated.h"

class ABRPlayerCharacter;
class UBoxComponent;

UCLASS()
class TD6_BACKROOM_API ADeathZone : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADeathZone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> BoxCollision;

	UFUNCTION()
	void OnBeginOverlap( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
