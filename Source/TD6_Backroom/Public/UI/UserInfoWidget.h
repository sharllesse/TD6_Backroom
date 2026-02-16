// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "UserInfoWidget.generated.h"

class UButton;
class UImage;
class UTextBlock;
/**
 * 
 */
UCLASS()
class TD6_BACKROOM_API UUserInfoWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> UserName;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Presence;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Activity;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> UserAvatar;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> JoinButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> InviteButton;

	UPROPERTY(EditAnywhere)
	FVector2D ImageSize;
	
	
	TFunction<void()> JoinButtonOnClickedEvent;
	TFunction<void()> InviteButtonOnClickedEvent;
	
	UFUNCTION()
	void JoinButtonCallback() const;
	UFUNCTION()
	void InviteButtonCallback() const;
public:
	virtual void NativeConstruct() override;
	
	void SetActivity(const FString & InActivity) const;
	void SetPresence(const FString & InPresence, const FColor& ColorPresence) const;
	void SetUserName(const FString & InUserName) const;
	void SetUserAvatar(UTexture2DDynamic* InUserAvatar) const;
	void SetCanBeJoin(bool bCanJoin) const;
	void SetCanBeInvited(bool bCanInvite) const;
	
	template<typename Func>
	void SetupJoinButton(Func&& ButtonDelegate)
	{
		JoinButtonOnClickedEvent = Forward<Func>(ButtonDelegate);
	}

	template<typename Func>
	void SetupInviteButton(Func&& ButtonDelegate)
	{
		InviteButtonOnClickedEvent = Forward<Func>(ButtonDelegate);
	}
};
