// Fill out your copyright notice in the Description page of Project Settings.

#include "ActorComponent/InteractionComponent.h"
#include "Interface/Interactable.h"
// Make sure to include DrawDebugHelpers.h if not already included in your header or PCH
// #include "DrawDebugHelpers.h" 

// Sets default values for this component's properties
UInteractionComponent::UInteractionComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;

    SetIsReplicatedByDefault(true);
    // ...
}

// Called when the game starts
void UInteractionComponent::BeginPlay()
{
    Super::BeginPlay();
    // ...
}

FHitResult UInteractionComponent::MakeVisibilityTrace() const
{
    AActor* Owner = GetOwner();
    if (!Owner)
    {
       return {};
    }

    FVector Position{};
    FRotator Rotation{};
    Owner->GetActorEyesViewPoint(Position, Rotation);

    FVector Start{Position},
    End{(Position + Rotation.Vector() * PickUpRange)};
    
    FHitResult Result;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Owner);
    
    FCollisionShape SphereShape = FCollisionShape::MakeSphere(SphereRadius);

    GetWorld()->SweepSingleByChannel(Result, Start, End, FQuat::Identity, ECC_Visibility, SphereShape, Params);
    
    if (bDebug)
    {
       IF_WITH_EDITOR(
       DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.0f, 0, 1.0f);
       FVector DebugSphereLocation = Result.bBlockingHit ? Result.Location : End;
       DrawDebugSphere(GetWorld(), DebugSphereLocation, SphereRadius, 12, FColor::Red, false, 2.0f, 0, 1.0f);
          ,)
    }
    return Result;
}

void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (auto Pawn = Cast<APawn>(GetOwner()); !Pawn->IsLocallyControlled())
       return;
    
    const auto Result = MakeVisibilityTrace();

    if (auto Interactable = Cast<IInteractable>(Result.GetActor()))
    {
       if (Interactable != Target.GetInterface())
       {
          if (Target.GetInterface())
          {
             Target->OnEndFocus();
          }
          NotifyCanInteractEvent.ExecuteIfBound(true, Interactable);
          Interactable->OnBeginFocus();
       }     
       Target.SetInterface(Interactable);
       Target.SetObject(Interactable->_getUObject());
    }
    else
    {
       if (Target.GetInterface())
       {
          NotifyCanInteractEvent.ExecuteIfBound(false, nullptr);
          Target->OnEndFocus();
       }
       Target = {};
    }
}

TScriptInterface<IInteractable> UInteractionComponent::GetTarget() const
{
    return Target;
}

IInteractable* UInteractionComponent::GetRawTarget() const
{
    return Target.GetInterface();
}

bool UInteractionComponent::CanInteract() const
{
    return Target.GetInterface() != nullptr;
}

void UInteractionComponent::TryInteract_Implementation(const TScriptInterface<IInteractable>& InTarget)
{
    if (!InTarget)
    {
       return;
    }
    const auto Result = MakeVisibilityTrace();
    IInteractable* HitTarget = Cast<IInteractable>(Result.GetActor());
    
    if (HitTarget == InTarget.GetInterface())
    {
       InTarget->OnInteract(GetOwner());
       OnInteract.ExecuteIfBound(InTarget.GetInterface());
       return;
    }
    return;
}