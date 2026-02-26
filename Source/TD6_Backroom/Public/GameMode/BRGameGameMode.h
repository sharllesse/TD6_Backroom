// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BRGameGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TD6_BACKROOM_API ABRGameGameMode : public AGameModeBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FVector2D VhsToCollectRange{8,12};

	int VhsToCollect{0};
	bool bHasAllVhs{false};
	FDelegateHandle UpdateVhsStateDelegate;


	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void CheckIfHasAllVhs();
public:

	int GetVhsObjective() const;
};
