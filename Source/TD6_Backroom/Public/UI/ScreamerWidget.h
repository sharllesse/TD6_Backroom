// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScreamerWidget.generated.h"

class UMediaSource;
class UMediaTexture;
class UMediaSoundComponent;
class UImage;
class UMediaPlaylist;
class UMediaPlayer;

DECLARE_DELEGATE(FOnScreamerEndSignature)

/**
 * 
 */
UCLASS()
class TD6_BACKROOM_API UScreamerWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ScreamerImage;
	
	
	UPROPERTY(EditAnywhere, Category = "Horror System|Media")
	TObjectPtr<UMediaPlayer> ScreamerMediaPlayer;

	UPROPERTY(EditAnywhere, Category = "Horror System|Media")
	TObjectPtr<UMediaSource> ScreamerMediaSource;

	UPROPERTY(EditAnywhere, Category = "Horror System|Media")
	TObjectPtr<UMediaTexture> ScreamerMediaTexture;
	
	UFUNCTION()
	void OnEndReached();

	virtual void NativeConstruct() override;
public:

	FOnScreamerEndSignature OnScreamerEnd;
	
	void TriggerScreamer();

	UMediaPlayer* GetMediaPlayer() const;

	void SetupScreamer(UMediaSoundComponent* SoundComponent);

	bool IsPlaying() const;
};
