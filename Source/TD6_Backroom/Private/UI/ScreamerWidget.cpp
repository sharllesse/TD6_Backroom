// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ScreamerWidget.h"

#include "MediaSoundComponent.h"
#include "MediaTexture.h"
#include "Components/Image.h"
#include "GameInstance/BRGameInstance.h"
#include "Runtime/MediaAssets/Public/MediaPlayer.h"
#include "Save/OptionSettingsSave.h"


void UScreamerWidget::OnEndReached()
{
	OnScreamerEnd.ExecuteIfBound();
	RemoveFromParent();
}

void UScreamerWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetVisibility(ESlateVisibility::Hidden);
	ScreamerMediaPlayer->OnEndReached.AddDynamic(this, &UScreamerWidget::OnEndReached);
}


void UScreamerWidget::TriggerScreamer()
{
	if (!ScreamerMediaPlayer || !ScreamerMediaSource || !ScreamerImage)
	{
		return;
	}
	SetVisibility(ESlateVisibility::Visible);

		

	ScreamerMediaPlayer->Rewind();
	ScreamerMediaPlayer->Play();
}

UMediaPlayer* UScreamerWidget::GetMediaPlayer() const
{
	return ScreamerMediaPlayer;
}

void UScreamerWidget::SetupScreamer(UMediaSoundComponent* SoundComponent)
{
	if (!SoundComponent || !ScreamerMediaPlayer || !ScreamerMediaTexture || !ScreamerMediaSource)
	{
		UE_LOG(LogTemp, Error, TEXT("Error in the setup of the screamer"));
		return;
	}
		
	SoundComponent->SetMediaPlayer(ScreamerMediaPlayer);
	ScreamerMediaTexture->SetMediaPlayer(ScreamerMediaPlayer);
	ScreamerMediaPlayer->OpenSource(ScreamerMediaSource);
	
}

bool UScreamerWidget::IsPlaying() const
{
	return ScreamerMediaPlayer->IsPlaying();
}
