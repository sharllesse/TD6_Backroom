// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/ExitZone.h"

#include "EventBus.h"
#include "Actor/ActorGameplayTags.h"
#include "Character/BRPlayerCharacter.h"


// Sets default values
AExitZone::AExitZone()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	ExitBox = CreateDefaultSubobject<UBoxComponent>("Exit Box");
	ExitBox->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AExitZone::BeginPlay()
{
	Super::BeginPlay();
	
	if(!HasAuthority())
	{
		return;
	}
	
	ExitBox->OnComponentBeginOverlap.AddDynamic(this, &AExitZone::OnComponentBeginOverlap);
	ExitBox->OnComponentEndOverlap.AddDynamic(this, &AExitZone::OnComponentEndOverlap);
	UEventBus::LockSignature<AActor*>(this, Actor_ExitZone_Callback_OnPlayerEnter);
	UEventBus::LockSignature<AActor*>(this, Actor_ExitZone_Callback_OnPlayerLeave);
	
}

void AExitZone::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if(!HasAuthority())
	{
		return;
	}
	
	
	UEventBus::UnlockSignature(this, Actor_ExitZone_Callback_OnPlayerEnter);
	UEventBus::UnlockSignature(this, Actor_ExitZone_Callback_OnPlayerLeave);
}

void AExitZone::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{	
	if (HasAuthority() && OtherActor->IsA<ABRPlayerCharacter>())
	{
		UEventBus::Broadcast(this, Actor_ExitZone_Callback_OnPlayerEnter, OtherActor);
	}
}

void AExitZone::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (HasAuthority() && OtherActor->IsA<ABRPlayerCharacter>())
	{
		UEventBus::Broadcast(this, Actor_ExitZone_Callback_OnPlayerLeave, OtherActor);
	}
}

// Called every frame
void AExitZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

