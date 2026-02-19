// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BRLobbyPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class TD6_BACKROOM_API ABRLobbyPlayerState : public APlayerState
{
	GENERATED_BODY()
protected:
	UPROPERTY(ReplicatedUsing = OnRep_IsReady, BlueprintReadOnly, Category = "Lobby")	
	bool bIsReady{false};
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION()
	void OnRep_IsReady();
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:

	void SetIsReady(bool bNewReady);
	bool GetIsReady() const;
};
