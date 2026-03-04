// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/LevelLight.h"


// Sets default values
ALevelLight::ALevelLight()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	RootComponent= CreateDefaultSubobject<USceneComponent>("Root");
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Neon");
	Mesh->SetupAttachment(RootComponent);
	Light = CreateDefaultSubobject<URectLightComponent>("Light");
	Light->SetupAttachment(Mesh);
	
	bReplicates = true;
}

// Called when the game starts or when spawned
void ALevelLight::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		DelayFlickeringStart.Schedule([this]
		{
			ChanceToFlickerTimer.Schedule([this]
		{
			if (bIsFlickering)
			{
				return;
			}
	
			if (FMath::RandRange(0.f,100.f) <= FlickeringChance)
			{
				bIsFlickering = true;
			}
	
			if (!bIsFlickering)
			{
				return;
			}
	
			int	FlickeringIteration = FMath::RandRange(2,5);
			FlickeringIteration += FlickeringIteration % 2 == 0 ? 0 : 1;
			FlickeringTimer.Schedule([this, FlickeringIteration]
			{
				LaunchFlickeringTimer(FlickeringIteration - 1);
				}, FTimerParameters{.bIsLooping = false, .Rate = static_cast<float>(FMath::RandRange(FlickeringRateChance.X, FlickeringRateChance.Y))});
		}, 
		{.bIsLooping = true, .Rate = CheckForFlickerRate});
		}, {.bIsLooping = false, .Rate = FMath::RandRange(0.f,25.f)});
		
		
		
	}
	
	
}

void ALevelLight::LaunchFlickeringTimer_Implementation(int FlickeringIteration)
{
	Mesh->SetVisibility(!Mesh->IsVisible());
	Light->SetVisibility(!Light->IsVisible());
	
	if (!HasAuthority())
	{
		return;
	}
	
	if (FlickeringIteration > 0)
	{
		FlickeringTimer.Schedule([this, FlickeringIteration]
		{
			LaunchFlickeringTimer(FlickeringIteration - 1);
		}, FTimerParameters{.bIsLooping = false, .Rate = static_cast<float>(FMath::RandRange(FlickeringRateChance.X, FlickeringRateChance.Y))});
	}
	else
	{
		bIsFlickering = false;
	}
}

// Called every frame
void ALevelLight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

