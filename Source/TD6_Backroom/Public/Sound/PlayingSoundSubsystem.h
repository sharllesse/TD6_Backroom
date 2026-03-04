// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PlayingSoundSubsystem.generated.h"

/**
 *	
 */
UCLASS()
class TD6_BACKROOM_API UPlayingSoundSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

	TMap<FString, TObjectPtr<UAudioComponent>> CurrentPlayingSounds;
	
public:
	static ThisClass* Get(const UObject* WorldContext);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	void PlaySoundAttached(USoundBase* Sound, USceneComponent* AttachToComponent, FName AttachPointName = NAME_None, FVector Location = FVector(ForceInit), FRotator Rotation = FRotator::ZeroRotator, EAttachLocation::Type LocationType = EAttachLocation::KeepRelativeOffset, bool bStopWhenAttachedToDestroyed = false, float VolumeMultiplier = 1.f, float PitchMultiplier = 1.f, float StartTime = 0.f, USoundAttenuation* AttenuationSettings = nullptr, USoundConcurrency* ConcurrencySettings = nullptr, bool bAutoDestroy = true);

	void PlaySound2D(USoundBase* Sound, float VolumeMultiplier = 1.f, float PitchMultiplier = 1.f, float StartTime = 0.f, USoundConcurrency* ConcurrencySettings = nullptr, bool bPersistAcrossLevelTransition = false, bool bAutoDestroy = true);
	
	void ModifyAllPlayingSoundVolume(float NewVolume);
	
	void StopSound(USoundBase* Sound);
private:
	void OnCurrentPlayingSoundFinished(UAudioComponent* AudioComponent);
};
