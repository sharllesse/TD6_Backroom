// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/GameModeBase.h"
#include "Utils/EventBusDelegateHandler.h"
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
	int CurrentPlayerNumberInExitZone{0};
	bool bAllPlayerAreInExitZone{false};
	
	EventBusDelegateHandler DelegateHandler;


	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void CheckIfHasAllVhs();
	void CheckIfAllPlayerAreInExitZone();
public:

	int GetVhsObjective() const;
};
