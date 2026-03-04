// Fill out your copyright notice in the Description page of Project Settings.


#include "Sound/PlayingSoundSubsystem.h"

#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

UPlayingSoundSubsystem::ThisClass* UPlayingSoundSubsystem::Get(const UObject* WorldContext)
{
	if (UWorld* World{ GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull) })
	{
		return World->GetSubsystem<ThisClass>();
	}

	return nullptr;
}

void UPlayingSoundSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	//UGameplayStatics::PushSoundMixModifier()
}

void UPlayingSoundSubsystem::PlaySoundAttached(USoundBase* Sound, USceneComponent* AttachToComponent,
                                               FName AttachPointName, FVector Location, FRotator Rotation, EAttachLocation::Type LocationType,
                                               bool bStopWhenAttachedToDestroyed, float VolumeMultiplier, float PitchMultiplier, float StartTime,
                                               USoundAttenuation* AttenuationSettings, USoundConcurrency* ConcurrencySettings, bool bAutoDestroy)
{
	UAudioComponent* AudioComponent =
		UGameplayStatics::SpawnSoundAttached(
			Sound, AttachToComponent, AttachPointName, Location, Rotation, LocationType, bStopWhenAttachedToDestroyed,
			VolumeMultiplier, PitchMultiplier, StartTime, AttenuationSettings, ConcurrencySettings, bAutoDestroy);

	AudioComponent->OnAudioFinishedNative.AddUObject(this, &UPlayingSoundSubsystem::OnCurrentPlayingSoundFinished);
	
	CurrentPlayingSounds.Add(AudioComponent->GetSound()->GetName(), AudioComponent);
}

void UPlayingSoundSubsystem::PlaySound2D(USoundBase* Sound, float VolumeMultiplier,
	float PitchMultiplier, float StartTime, USoundConcurrency* ConcurrencySettings, bool bPersistAcrossLevelTransition,
	bool bAutoDestroy)
{
	UAudioComponent* AudioComponent =
		UGameplayStatics::SpawnSound2D(this, Sound, VolumeMultiplier, PitchMultiplier,
			StartTime, ConcurrencySettings, bPersistAcrossLevelTransition, bAutoDestroy);

	AudioComponent->OnAudioFinishedNative.AddUObject(this, &UPlayingSoundSubsystem::OnCurrentPlayingSoundFinished);
	
	CurrentPlayingSounds.Add(AudioComponent->GetSound()->GetName(), AudioComponent);
}

void UPlayingSoundSubsystem::ModifyAllPlayingSoundVolume(float NewVolume)
{
	for (const auto& CurrentPlayingSound : CurrentPlayingSounds)
	{
		CurrentPlayingSound.Value->SetVolumeMultiplier(NewVolume);
	}
}

void UPlayingSoundSubsystem::StopSound(USoundBase* Sound)
{
	if (auto* AudioComponent = CurrentPlayingSounds.Find(Sound->GetName()))
	{
		(*AudioComponent)->Stop();
	}
}

void UPlayingSoundSubsystem::OnCurrentPlayingSoundFinished(UAudioComponent* AudioComponent)
{
	CurrentPlayingSounds.Remove(AudioComponent->GetSound()->GetName());
}
