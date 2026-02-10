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
								"[Args: FName SessionName, bool bWasSuccessful]")

UE_DEFINE_GAMEPLAY_TAG_COMMENT(Online_Callback_OnDestroySessionRequested, "Online.Callback.OnDestroySessionRequested", "")

UE_DEFINE_GAMEPLAY_TAG_COMMENT(Online_Callback_OnDestroySessionCompleted, "Online.Callback.OnDestroySessionCompleted", "")

UE_DEFINE_GAMEPLAY_TAG_COMMENT(Online_Callback_OnJoinSessionCompleted, "Online.Callback.OnJoinSessionCompleted", "")

UE_DEFINE_GAMEPLAY_TAG_COMMENT(Online_Callback_OnExternalUIChange, "Online.Callback.OnExternalUIChange", "")