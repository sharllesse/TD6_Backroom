// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FriendListWidget.h"

#include "EventBus.h"
#include "Online/BROnlineGameTags.h"
#include "Online/BROnlineSubsystem.h"
#include "UI/UserInfoWidget.h"


void UFriendListWidget::UpdateUser(const TArray<TSharedRef<FOnlineFriend>>& FriendList)
{
	for (auto Element : FriendList)
	{
		UE_LOG(LogTemp, Error, TEXT("%s"), *Element->GetDisplayName());
	}

	
	auto OnlineSubsystem = GetGameInstance()->GetSubsystem<UBROnlineSubsystem>();
	
	OnlineSubsystem->RetrievedAvatarTexture(OnlineSubsystem->GetAvatarURL(FriendList[0]), FriendList[0]->GetUserId());
	UEventBus::AddLambda(this, Online_Callback_OnAvatarTextureRetrieved, [&](UTexture2DDynamic* Texture, FUniqueNetIdWeakPtr UserId)
	{
		UE_LOG(LogTemp, Error, TEXT("is null ? %d"), (int)(Texture == nullptr));

		LocalPlayer->SetUserAvatar(Texture);
	});
	
}
