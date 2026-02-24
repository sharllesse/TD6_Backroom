// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/AICharacter_Base.h"
#include "AICharacter_Bacteria.generated.h"

UCLASS()
class TD6_BACKROOM_API AAICharacter_Bacteria : public AAICharacter_Base
{
	GENERATED_BODY()

public:
	AAICharacter_Bacteria();

protected:
	virtual void BeginPlay() override;
};
