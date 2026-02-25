// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/PickableItem.h"

#include "Chain.h"
#include "GameState/BRGameGameState.h"
#include "Items/ItemData.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
APickableItem::APickableItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	MeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void APickableItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickableItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APickableItem::OnBeginFocus()
{
	IInteractable::OnBeginFocus();
	MeshComponent->SetOverlayMaterial(HighlightMaterial);
}

void APickableItem::OnEndFocus()
{
	IInteractable::OnEndFocus();
	MeshComponent->SetOverlayMaterial(nullptr);
}

void APickableItem::OnInteract(AActor* InInstigator)
{
	IInteractable::OnInteract(InInstigator);

	if (auto OptionalItemData = UItemDataTable::GetItemData(Type))
	{
		if (auto& ItemData = *OptionalItemData; ItemData.bIsShared)
		{
			Chain::Execute(GetWorld()->GetGameState<ABRGameGameState>(), [&](ABRGameGameState* GameState)
			{
				ItemData.Count = 1;
				GameState->AddItem(ItemData);
			});
		}
		else
		{
		}		
	}
	Destroy();
}

