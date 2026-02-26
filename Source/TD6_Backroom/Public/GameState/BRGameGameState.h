// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Items/ItemData.h"
#include "BRGameGameState.generated.h"

/**
 * 
 */
UCLASS()
class TD6_BACKROOM_API ABRGameGameState : public AGameStateBase
{
	GENERATED_BODY()

protected:

	UPROPERTY(Replicated)
	TArray<FItemData> SharedInventory;



	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastNotifyItemAdded(const FItemData& NewItem);
public:
	void AddItem(const FItemData& NewItem);
	void RemoveItem(const FItemData& NewItem);
	void ClearInventory();
	TOptional<FItemData> GetItem(const FGameplayTag& Tag) const;
	const TArray<FItemData>& GetSharedInventory() const;
	
	UPROPERTY(Replicated)
	int VhsToCollect{0};
	
	UPROPERTY(ReplicatedUsing = "OnObjectivesCompleted_Rep")
	bool bAllObjectiveIsCompleted{false};

	
	UFUNCTION()
	void OnObjectivesCompleted_Rep() const;
	
};
