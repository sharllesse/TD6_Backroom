// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExitDoor.generated.h"

UCLASS()
class TD6_BACKROOM_API AExitDoor : public AActor
{
	GENERATED_BODY()

public:
	AExitDoor();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UStaticMeshComponent> DoorMesh;

	FDelegateHandle OnObjectiveCompletedHandle;
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintNativeEvent)
	void MakeRotation();
public:
	virtual void Tick(float DeltaTime) override;
};
