// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/InteractionComponent.h"

#include "Interface/Interactable.h"


// Sets default values for this component's properties
UInteractionComponent::UInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

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
	End{Position + Rotation.GetComponentForAxis(EAxis::X) * PickUpRange};
	
	FHitResult Result;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);
	GetWorld()->LineTraceSingleByChannel(Result, Start, End, ECC_Visibility);
	return Result;
}

// Called every frame
void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	const auto Result = MakeVisibilityTrace();
	
	Target = Cast<IInteractable>(Result.GetActor());
	
	if (Target)
	{
		NotifyCanInteractEvent.ExecuteIfBound(true, Target);
		return;
	}
	NotifyCanInteractEvent.ExecuteIfBound(false, nullptr);
}

IInteractable* UInteractionComponent::GetTarget() const
{
	return Target;
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

