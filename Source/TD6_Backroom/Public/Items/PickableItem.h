// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "Interface/Interactable.h"
#include "PickableItem.generated.h"

UCLASS()
class TD6_BACKROOM_API APickableItem : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	APickableItem();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag Type;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UMaterialInterface* HighlightMaterial;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void OnBeginFocus() override;
	virtual void OnEndFocus() override;
	virtual void OnInteract(AActor* InInstigator) override;
};
