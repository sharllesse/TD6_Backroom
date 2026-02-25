// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interface/Interactable.h"
#include "InteractionComponent.generated.h"


DECLARE_DELEGATE_TwoParams(NotifyCanInteractSignature, bool, IInteractable*)
DECLARE_DELEGATE_OneParam(OnInteractSignature, IInteractable*)

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TD6_BACKROOM_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UInteractionComponent();

protected:

	virtual void BeginPlay() override;


	FHitResult MakeVisibilityTrace() const;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;

	TScriptInterface<IInteractable> Target;

	UPROPERTY(EditDefaultsOnly)
	bool bDebug{false};
	
	UPROPERTY(EditDefaultsOnly)
	float PickUpRange{500.f};
public:

	NotifyCanInteractSignature NotifyCanInteractEvent;
	OnInteractSignature OnInteract;

	TScriptInterface<IInteractable> GetTarget() const;
	IInteractable* GetRawTarget() const;

	bool CanInteract() const;
	
	UFUNCTION(Server, Reliable)
	void TryInteract(const TScriptInterface<IInteractable>& InTarget);
};
