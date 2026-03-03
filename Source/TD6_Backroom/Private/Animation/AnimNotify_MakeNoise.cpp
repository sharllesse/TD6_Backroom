// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_MakeNoise.h"
#include "Perception/AISense_Hearing.h"

FString UAnimNotify_MakeNoise::GetNotifyName_Implementation() const
{
	return NoiseName.ToString();
}

void UAnimNotify_MakeNoise::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	AActor* Instigator{ MeshComp->GetOwner() };

	FVector NoiseLocation;
	if (!BoneLocation.IsNone())
	{
		NoiseLocation = MeshComp->GetBoneLocation(BoneLocation);
	}
	else
	{
		NoiseLocation = Instigator->GetActorLocation();
	}
	
	UAISense_Hearing::ReportNoiseEvent(Instigator, NoiseLocation, Loudness, Instigator, MaxRange, NoiseName);
}
