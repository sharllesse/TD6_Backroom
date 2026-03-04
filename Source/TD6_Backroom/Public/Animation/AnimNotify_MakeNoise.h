// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_MakeNoise.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Make Noise")
class TD6_BACKROOM_API UAnimNotify_MakeNoise : public UAnimNotify
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	FName NoiseName{ "Default Noise" };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	FName BoneLocation{ NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	float Loudness{ 1.f };
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	float MaxRange{ 1500.f };
public:
	virtual FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
