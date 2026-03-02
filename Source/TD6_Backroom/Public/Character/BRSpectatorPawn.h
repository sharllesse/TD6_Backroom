// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h" 
#include "Camera/CameraComponent.h"
#include "GameFramework/SpectatorPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "BRSpectatorPawn.generated.h"

UCLASS()
class TD6_BACKROOM_API ABRSpectatorPawn : public ASpectatorPawn
{
	GENERATED_BODY()

	friend class ABRPlayerController;
public:
	// Sets default values for this pawn's properties
	ABRSpectatorPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	
	int CurrentPlayerSpectateIndex;

	TWeakObjectPtr<APawn> CurrentPawnSpectated;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	enum class IterationMethode
	{
		Next,
		Previous,
		Init
	};
	
	void SpectateNextPlayer(IterationMethode InIterationMethode);
};
