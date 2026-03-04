// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/DeathZone.h"

#include "Character/BRPlayerCharacter.h"
#include "Components/BoxComponent.h"


// Sets default values
ADeathZone::ADeathZone()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	BoxCollision = CreateDefaultSubobject<UBoxComponent>("Box");
	BoxCollision->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ADeathZone::BeginPlay()
{
	Super::BeginPlay();

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ADeathZone::OnBeginOverlap);
}

void ADeathZone::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (auto Player = Cast<ABRPlayerCharacter>(OtherActor))
	{
		Player->EnableRagdoll();
	}
}


// Called every frame
void ADeathZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

