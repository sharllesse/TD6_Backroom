// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TimerHolder.h"
#include "Components/RectLightComponent.h"
#include "GameFramework/Actor.h"
#include "LevelLight.generated.h"

UCLASS()
class TD6_BACKROOM_API ALevelLight : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALevelLight();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Light")
	TObjectPtr<UStaticMeshComponent> Mesh;
	
	UPROPERTY(EditDefaultsOnly, Category = "Light")
	TObjectPtr<URectLightComponent> Light;
	
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly)
	float FlickeringChance{1.f};
	UPROPERTY(EditDefaultsOnly)
	FVector2D FlickeringRateChance{0.1f,0.3f};
	UPROPERTY(EditDefaultsOnly)
	float CheckForFlickerRate{0.1f};
	
	bool bIsFlickering{false};
	FTimerHolder FlickeringTimer;
	FTimerHolder ChanceToFlickerTimer;
	FTimerHolder DelayFlickeringStart;
	
	UFUNCTION(NetMulticast, Reliable)
	void LaunchFlickeringTimer(int FlickeringIteration);
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
