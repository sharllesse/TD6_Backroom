// Copyright (c) Logicraft Interactive. All Rights Reserved.


#include "Online/BROnlineGameTags.h"

#define ON_LOGIN_STATUS_CHANGED_DESCRIPTION												 								    \
"A callback made to know when the user login status has changed.\n"						  								    \
"NotLoggedIn means that nobody is logged in and that the user is unknown, even locally.\n"								    \
"On the other hand, UsingLocalProfile means that the user is not logged into the service being used but is known locally.\n"\
"And LoggedIn means that the user is logged in and known locally and on the network.\n"								  	    \
"[Args: int32 LocalUserNum, ELoginStatus::Type OldStatus, ELoginStatus::Type NewStatus, const FUniqueNetId& NewId]"	    \

UE_DEFINE_GAMEPLAY_TAG_COMMENT(Online_Callback_OnLoginComplete, "Online.Callback.OnLoginComplete",
								"A callback made to know if the login process is a success or not.\n"
								"[Args: int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error]")

UE_DEFINE_GAMEPLAY_TAG_COMMENT(Online_Callback_OnLogoutComplete, "Online.Callback.OnLogoutComplete",
								"A callback made to know if the logout process is a success or not.\n"
								"[Args: int32 LocalUserNum, bool bWasSuccessful]")

UE_DEFINE_GAMEPLAY_TAG_COMMENT(Online_Callback_OnLoginStatusChanged, "Online.Callback.OnLoginStatusChanged", ON_LOGIN_STATUS_CHANGED_DESCRIPTION)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(Online_Callback_OnCreateSessionCompleted, "Online.Callback.OnCreateSessionCompleted",
								"A callback made to notify when a session creation has been successful or not.\n"
								"Be aware that before this callback is triggered, the session is not ready yet, even if it failed.\n"
								"[Args: const FString& SessionName, TWeakPtr<const FOnlineSessionSettings>, bool bWasSuccessful]")

UE_DEFINE_GAMEPLAY_TAG_COMMENT(Online_Callback_OnDestroySessionRequested, "Online.Callback.OnDestroySessionRequested",
								"A callback made to notify when a session destroying process has been requested.\n"
								"[Args: const FString& SessionName, int32 LocalUserNum]")

UE_DEFINE_GAMEPLAY_TAG_COMMENT(Online_Callback_OnDestroySessionCompleted, "Online.Callback.OnDestroySessionCompleted",
								"A callback made to notify when a session destroying process has been completed.\n"
								"Be aware that the process could be unsuccessful and SessionName could contain Online_Settings_Error.\n"
								"[Args: const FString& SessionName, bool bWasSuccessful]")

UE_DEFINE_GAMEPLAY_TAG_COMMENT(Online_Callback_OnFindSessionsCompleted, "Online.Callback.OnFindSessionsCompleted",
								"A callback made to notify when the search of sessions is completed.\n"
								"[Args: const TArray<FOnlineSessionSearchResult>& SessionsResult, bool bWasSuccessful]")	

UE_DEFINE_GAMEPLAY_TAG_COMMENT(Online_Callback_OnJoinSessionCompleted, "Online.Callback.OnJoinSessionCompleted",
								"A callback made to notify when the join session process is completed.\n"
								"[Args: EOnJoinSessionCompleteResult::Type Result]")

UE_DEFINE_GAMEPLAY_TAG_COMMENT(Online_Callback_OnSessionUserInviteAccepted, "Online.Callback.OnSessionUserInviteAccepted",
								"A callback that notify when the user has accepted an invitation or just joined a friend (Using the friend list).\n"
								"[Args: const bool bWasSuccessful, const int32 ControllerId, FUniqueNetIdPtr UserId, const FOnlineSessionSearchResult& InviteResult]")

UE_DEFINE_GAMEPLAY_TAG_COMMENT(Online_Callback_OnSessionStartCompleted, "Online.Callback.OnSessionStartCompleted",
								"A callback that notify when the session has started.\n"
								"[Args: bool bWasSuccessful]")

UE_DEFINE_GAMEPLAY_TAG_COMMENT(Online_Callback_OnSessionInviteReceived, "Online.Callback.OnSessionInviteReceived",
								"A callback that notify when an invite is received from a friend.\n"
								"[Args: const FUniqueNetId& UserId, const FUniqueNetId& FromId, const FString& AppId, const FOnlineSessionSearchResult& InviteResult]")

UE_DEFINE_GAMEPLAY_TAG_COMMENT(Online_Callback_OnRefreshSessionTimerFinish, "Online.Callback.OnRefreshSessionTimerFinish",
								"A callback made to notify when the refresh timer has finish.\n"
								"[Args: ]")

UE_DEFINE_GAMEPLAY_TAG_COMMENT(Online_Callback_OnPresenceReceived, "Online.Callback.OnPresenceReceived",
								"A callback made to notify when a friends presence has changed.\n"
								"[Args: const FUniqueNetId& UserId, const TSharedRef<FOnlineUserPresence>& Presence]")

UE_DEFINE_GAMEPLAY_TAG_COMMENT(Online_Callback_OnReadFriendsListCompleted, "Online.Callback.OnReadFriendsListCompleted",
								"A callback made to notify when the query friends list has completed.\n"
								"[Args: int32 LocalUserNum, bool bWasSuccessful, const TArray<TSharedRef<FOnlineFriend>>& OnlineFriends, const FString& ErrorStr]")

UE_DEFINE_GAMEPLAY_TAG_COMMENT(Online_Callback_OnAvatarTextureRetrieved, "Online.Callback.OnAvatarTextureRetrieved",
								"A callback that notify when the request to retrieved the avatar texture is a success or not.\n"
								"Be aware that if the retrieve process failed, Texture is equal to nullptr.\n"
								"[Args: UTexture2DDynamic* Texture, FUniqueNetIdWeakPtr UserId]")

UE_DEFINE_GAMEPLAY_TAG_COMMENT(Online_Callback_OnMainExternalUIOverlayChange, "Online.Callback.OnMainExternalUIOverlayOpen",
								"A callback that notify when the main external overlay (epic friend overlay) open or close.\n"
								"[Args: bool bIsOpening]")
