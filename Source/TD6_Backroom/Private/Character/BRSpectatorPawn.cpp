// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BRSpectatorPawn.h"

#include "Chain.h"
#include "EventBus.h"
#include "Character/BRCharacterGameplayTags.h"
#include "GameFramework/PlayerState.h"
#include "GameState/BRGameGameState.h"


// Sets default values
ABRSpectatorPawn::ABRSpectatorPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
    
    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("Spring arm");
    SpringArmComponent->SetupAttachment(RootComponent);
    
    CameraComponent = CreateDefaultSubobject<UCameraComponent>("Camera");
    CameraComponent->SetupAttachment(SpringArmComponent);

}

// Called when the game starts or when spawned
void ABRSpectatorPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABRSpectatorPawn::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    UE_LOG(LogTemp, Error, TEXT("ABRSpectatorPawn::PossessedBy"))
    SpectateNextPlayer(IterationMethode::Init);
}

// Called every frame
void ABRSpectatorPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (CurrentPawnSpectated.IsValid())
    {
        FVector NewPos{};
        FRotator NewRotation{};
        CurrentPawnSpectated->GetActorEyesViewPoint(NewPos,NewRotation);
        SetActorLocation(NewPos);
    }
    else
    {
        SpectateNextPlayer(IterationMethode::Next);
    }
}

// Called to bind functionality to input
void ABRSpectatorPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABRSpectatorPawn::SpectateNextPlayer(IterationMethode InIterationMethode)
{
    if (!IsLocallyControlled())
    {
        return;
    }

    Chain::Execute(GetWorld()->GetGameState<ABRGameGameState>(), [this, InIterationMethode](ABRGameGameState* GameState)
    {
        const int32 TotalPlayers = GameState->PlayerArray.Num(); 
        if (TotalPlayers <= 0)
        {
            return;
        }

        int32 CurrentIndex = GameState->PlayerArray.IsValidIndex(CurrentPlayerSpectateIndex) ? CurrentPlayerSpectateIndex : 0;
        
        APlayerController* PC = Cast<APlayerController>(GetController());
        if (!PC)
        {
            return;
        }

        
        if (InIterationMethode == IterationMethode::Init)
        {
            for (auto TargetPlayerState : GameState->PlayerArray)
            {
                if (TargetPlayerState && !TargetPlayerState->IsSpectator())
                {
                    APawn* TargetPawn = TargetPlayerState->GetPawn();
                
                    if (TargetPawn && TargetPawn != this)
                    {
                        if (TargetPawn)
                        {
                            PC->SetViewTargetWithBlend(TargetPawn, 0.2f, EViewTargetBlendFunction::VTBlend_Cubic);
                        } 
                        CurrentPlayerSpectateIndex = CurrentIndex;
                        UE_LOG(LogTemp, Error, TEXT("Broadcast"));
                        UEventBus::Broadcast<const APlayerState*>(this, Character_Callback_OnPlayerSwitchSpectate, TargetPlayerState);
                        
                        break;
                    }
                }
            }
            return;
        }
        
        const int32 Step = (InIterationMethode == IterationMethode::Next) ? 1 : -1;
        APawn* ValidPawnToSpectate = nullptr;

        for (int32 i = 0; i < TotalPlayers; ++i)
        {
            CurrentIndex = (CurrentIndex + Step + TotalPlayers) % TotalPlayers;
            
            APlayerState* TargetPlayerState = GameState->PlayerArray[CurrentIndex];
            
            if (TargetPlayerState && !TargetPlayerState->IsSpectator())
            {
                APawn* TargetPawn = TargetPlayerState->GetPawn();
                
                if (TargetPawn && TargetPawn != this)
                {
                    ValidPawnToSpectate = TargetPawn;
                    CurrentPlayerSpectateIndex = CurrentIndex; 
                    UE_LOG(LogTemp, Error, TEXT("Broadcast"));
                    UEventBus::Broadcast<const APlayerState*>(this, Character_Callback_OnPlayerSwitchSpectate, TargetPlayerState);
                    break;
                }
            }
        }
        
        if (ValidPawnToSpectate)
        {
            CurrentPawnSpectated = ValidPawnToSpectate;
            CurrentPlayerSpectateIndex = CurrentIndex;
            PC->SetViewTargetWithBlend(this);
        }
        return;
    });
}