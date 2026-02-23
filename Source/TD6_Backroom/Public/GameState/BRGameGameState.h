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
	TArray<FItemData> ItemMap;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
public:
};
