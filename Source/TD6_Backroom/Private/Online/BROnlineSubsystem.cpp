// Copyright (c) Logicraft Interactive. All Rights Reserved.

#include "TD6_Backroom/Public/Online/BROnlineSubsystem.h"

#include "EventBus.h"
#include "LogCategory.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#include "Blueprint/AsyncTaskDownloadImage.h"
#include "Online/BROnlineGameTags.h"
#include "Online/BROnlineSettings.h"

#include "Interfaces/OnlinePresenceInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Online/Http/BRAsyncTaskDownloadImage.h"
#include "Online/OnlineSessionNames.h"
#include "eos_ui.h"
#include "eos_sdk.h"
#include "IOnlineSubsystemEOS.h"
#include "IEOSSDKManager.h"

static EOS_NotificationId OverlayNotifId = EOS_INVALID_NOTIFICATIONID;

#define ONLINE_LOG(Verbosity, Format, ...)\
	UE_LOG(LogOWOnline, Verbosity, Format, ##__VA_ARGS__)

UBROnlineSubsystem::ThisClass* UBROnlineSubsystem::Get(const UObject* WorldContext)
{
	if (UWorld* World{GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull)})
	{
		return World->GetGameInstance()->GetSubsystem<ThisClass>();
	}

	return nullptr;
}

void UBROnlineSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	Collection.InitializeDependency<UEventBus>();

	Internal_RegisterDelegates();
	Internal_LockCallbacksSignature();
	
	if (UBROnlineSettings::Get()->bAutoLogin)
	{
		Login();
	}
}

void UBROnlineSubsystem::Deinitialize()
{
	Super::Deinitialize();

	Internal_ClearDelegates();
}

bool UBROnlineSubsystem::Login(bool bUseCommandLine, const FOnlineAccountCredentials& OnlineAccountCredentials) const
{
	return Internal_ExecuteOnValidContext(
	[bUseCommandLine, &OnlineAccountCredentials](const IOnlineIdentityPtr& IdentityInterface)
	{
		if (bUseCommandLine)
		{
			FString AuthType;
			auto F = FCommandLine::Get();
			FParse::Value(F, TEXT("AUTH_TYPE="), AuthType);
			if (!AuthType.IsEmpty())
			{
				ONLINE_LOG(
					Log,
					TEXT(
						"Login: Attempting to connect using command arguments [Auth Type = %s]"
					), *AuthType)
				return IdentityInterface->AutoLogin(0);
			}
		}

		ONLINE_LOG(Log,
			TEXT(
				"Login: Attempting to connect using online account credentials information [Information : %s]"
			), *OnlineAccountCredentials.ToDebugString())
		return IdentityInterface->Login(0, OnlineAccountCredentials);
	}, Online::GetIdentityInterface(GetWorld()));
}

bool UBROnlineSubsystem::Logout()
{
	return Internal_ExecuteOnValidContext(
	[this](const IOnlineIdentityPtr& IdentityInterface)
	{
#if !UE_BUILD_SHIPPING
		IOnlineSubsystem* OSS = Online::GetSubsystem(GetWorld());
		
		if (const auto UniqueId{ IdentityInterface->GetUniquePlayerId(0) })
		{
			ONLINE_LOG(Log,
				TEXT(
					"Logout: Attempting to logout from %s [User Name: %s]"
				), *OSS->GetSubsystemName().ToString(), *IdentityInterface->GetPlayerNickname(*UniqueId))	
		}
#endif
		
		return IdentityInterface->Logout(0);
	}, Online::GetIdentityInterface(GetWorld()));
}

bool UBROnlineSubsystem::CreateSession(int32 SessionMaxConnections, const FString& SessionName, bool bIsPrivate)
{
	const UWorld* World{ GetWorld() };
	
	return Internal_ExecuteOnValidContext(
	[this, SessionMaxConnections, SessionName, bIsPrivate]
	(const IOnlineSessionPtr& SessionInterface, const IOnlineIdentityPtr& IdentityInterface) mutable 
	{
		const FUniqueNetIdPtr UniquePlayerId{ IdentityInterface->GetUniquePlayerId(0) };

		FOnlineSessionSettings OnlineSessionSettings;
		OnlineSessionSettings.bAllowJoinInProgress = false;
		OnlineSessionSettings.bAllowJoinViaPresenceFriendsOnly = !bIsPrivate;
		OnlineSessionSettings.bAllowJoinViaPresence = !bIsPrivate;
		OnlineSessionSettings.bAllowInvites = true;
		OnlineSessionSettings.bUsesPresence = true;
		OnlineSessionSettings.bShouldAdvertise = !bIsPrivate;
		OnlineSessionSettings.bUseLobbiesIfAvailable = true;
		OnlineSessionSettings.Set(Online_Settings_Session_Name, SessionName, EOnlineDataAdvertisementType::ViaOnlineService);
		OnlineSessionSettings.Set(SETTING_ACTIVITY_SESSION, true, EOnlineDataAdvertisementType::ViaOnlineService);
		OnlineSessionSettings.Set(SETTING_MULTIPLAYER_VISIBILITY, bIsPrivate ? 0 : 1, EOnlineDataAdvertisementType::ViaOnlineService);

		constexpr int32 MinimumConnections{ 1 };
		SessionMaxConnections = FMath::Clamp(SessionMaxConnections, MinimumConnections, MaxSessionConnections);
		if (bIsPrivate)
		{
			OnlineSessionSettings.NumPrivateConnections = SessionMaxConnections;
		}
		else
		{
			OnlineSessionSettings.NumPublicConnections = SessionMaxConnections;	
		}

		OnlineData.CurrentOnlineSessionSettings = MakeShared<FOnlineSessionSettings>(OnlineSessionSettings);
		
		return SessionInterface->CreateSession(*UniquePlayerId, NAME_GameSession, OnlineSessionSettings);
	}, Online::GetSessionInterface(World), Online::GetIdentityInterface(World));
}

bool UBROnlineSubsystem::DestroySession()
{
	return Internal_ExecuteOnValidContext(
	[this](const IOnlineSessionPtr& SessionInterface) 
	{
		return SessionInterface->DestroySession(NAME_GameSession);
	}, Online::GetSessionInterface(GetWorld()));
}

bool UBROnlineSubsystem::FindSessions()
{
	return Internal_ExecuteOnValidContext([this](const IOnlineSessionPtr& SessionInterface)
	{
		const TSharedPtr CurrentSearchedSessionSettings
			= OnlineData.CurrentSearchedSessionSettings
			= MakeShared<FOnlineSessionSearch>();
		CurrentSearchedSessionSettings->MaxSearchResults = 10;
		CurrentSearchedSessionSettings->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
		CurrentSearchedSessionSettings->QuerySettings.Set(SEARCH_MINSLOTSAVAILABLE, 1, EOnlineComparisonOp::GreaterThanEquals);

		return SessionInterface->FindSessions(0, CurrentSearchedSessionSettings.ToSharedRef());
	}, Online::GetSessionInterface(GetWorld()));
}

bool UBROnlineSubsystem::JoinSession(const FOnlineSessionSearchResult& DesiredSession)
{
	return Internal_ExecuteOnValidContext([&DesiredSession](const IOnlineSessionPtr& SessionInterface)
	{
		return SessionInterface->JoinSession(0, NAME_GameSession, DesiredSession);
	}, Online::GetSessionInterface(GetWorld()));
}

bool UBROnlineSubsystem::StartSession()
{
	return Internal_ExecuteOnValidContext([](const IOnlineSessionPtr& SessionInterface)
	{
		return SessionInterface->StartSession(NAME_GameSession);
	}, Online::GetSessionInterface(GetWorld()));
}

void UBROnlineSubsystem::LaunchRefreshSessionsTimer(float Rate)
{
	OnlineData.RefreshSessionsTimer
		.Schedule(this, &UBROnlineSubsystem::OnRefreshSessionTimerFinish,
		{
			.bIsLooping = true, .Rate = Rate, .FirstDelay = 0.f
		});
}

void UBROnlineSubsystem::StopRefreshSessionTimer()
{
	OnlineData.RefreshSessionsTimer.Clear();
}

bool UBROnlineSubsystem::QueryFriendList()
{
	return Internal_ExecuteOnValidContext(
	[this](const IOnlineFriendsPtr& FriendsInterface)
	{
		return FriendsInterface->ReadFriendsList(0,
			EFriendsLists::ToString(EFriendsLists::Default),
			FOnReadFriendsListComplete::CreateUObject(this, &UBROnlineSubsystem::OnReadFriendsListComplete));
	}, Online::GetFriendsInterface(GetWorld()));
}

FString UBROnlineSubsystem::GetAvatarURL(TSharedRef<FOnlineUser> OnlineUser)
{
	FString AvatarUrl;
	if (OnlineUser->GetUserAttribute("avatarUrl", AvatarUrl))
	{
		return AvatarUrl;
	}

	return FString{};
}

void UBROnlineSubsystem::RetrievedAvatarTexture(const FString& AvatarUrl, FUniqueNetIdRef UserId)
{
	if (AvatarUrl.IsEmpty())
	{
		UEventBus::Broadcast<UTexture2DDynamic*>(this, Online_Callback_OnAvatarTextureRetrieved, nullptr, UserId.ToWeakPtr());		
	}
	
	if (UBRAsyncTaskDownloadImage* AsyncTaskDownloadImage = UBRAsyncTaskDownloadImage::DownloadImage(AvatarUrl,
		UserId.ToWeakPtr()))
	{
		AsyncTaskDownloadImage->OnSuccess.AddUObject(this, &UBROnlineSubsystem::OnAvatarTextureRetrieved);
		AsyncTaskDownloadImage->OnFail.AddUObject(this, &UBROnlineSubsystem::OnAvatarTextureRetrieved);
	}
}

void UBROnlineSubsystem::OnLoggingCompleted(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId,
                                            const FString& Error)
{
	if (!bWasSuccessful)
	{
		ONLINE_LOG(Error, TEXT("Login: Login was unsuccessful due to a error [Error: %s]"), *Error)
	}
	else
	{
#if !UE_BUILD_SHIPPING
		Internal_ExecuteOnValidContext(
		[&UserId](const IOnlineIdentityPtr& IdentityInterface)
		{
			ONLINE_LOG(Log, TEXT("Login: Login was successful [User Name: %s]"),
						*IdentityInterface->GetPlayerNickname(UserId))
		}, Online::GetIdentityInterface(GetWorld()));
#endif
		
		if (UBROnlineSettings::Get()->bAutoQueryFriends)
		{
			QueryFriendList();
		}
	}
	
	UEventBus::Broadcast<int32, bool, const FUniqueNetId&, const FString&>
	(this, Online_Callback_OnLoginComplete, LocalUserNum, bWasSuccessful, UserId, Error);
}

void UBROnlineSubsystem::OnLogoutCompleted(int32 LocalUserNum, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		ONLINE_LOG(Error, TEXT("Logout: Logout was unsuccessful due to an unknown error"))
	}
	else
	{
		ONLINE_LOG(Log, TEXT("Logout: Logout was successful"))	
	}

	UEventBus::Broadcast(this, Online_Callback_OnLogoutComplete, LocalUserNum, bWasSuccessful);
}

void UBROnlineSubsystem::OnLoginStatusChanged(int32 LocalUserNum, ELoginStatus::Type OldStatus,
	ELoginStatus::Type NewStatus, const FUniqueNetId& NewId)
{
	ONLINE_LOG(Log,
		TEXT("Login: Login status has changed [OldStatus: %s, NewStatus: %s]."),
		ELoginStatus::ToString(OldStatus), ELoginStatus::ToString(NewStatus))
	
	UEventBus::Broadcast<int32, ELoginStatus::Type, ELoginStatus::Type, const FUniqueNetId&>
	(this, Online_Callback_OnLoginStatusChanged, LocalUserNum, OldStatus, NewStatus, NewId);
}

void UBROnlineSubsystem::OnCreateSessionCompleted(FName SessionName, bool bWasSuccessful)
{
	const UWorld* World{ GetWorld() };
	
	Internal_ExecuteOnValidContext(
	[this, bWasSuccessful, SessionName]
	(const TSharedPtr<FOnlineSessionSettings>& OnlineSessionSettings,
	const IOnlinePresencePtr& PresenceInterface,
	const IOnlineIdentityPtr& IdentityInterface,
	const IOnlineSessionPtr& SessionInterface)
	{
		FString SettingsSessionName;
		if (!OnlineSessionSettings->Get(Online_Settings_Session_Name, SettingsSessionName))
		{
			ONLINE_LOG(Error, TEXT("Session: Unable to retrieve session settings. Fallback to the error broadcast."))

			UEventBus::Broadcast<const FString&>(this, Online_Callback_OnCreateSessionCompleted,
			FString{ Online_Settings_Error }, TWeakPtr<const FOnlineSessionSettings>{ nullptr }, bWasSuccessful);
			return;
		}
		
		if (!bWasSuccessful)
		{
			ONLINE_LOG(Error, TEXT("Session: Creation of the session [Name: %s] has failed."), *SettingsSessionName)
			return;
		}

		//SessionInterface->StartSession(SessionName);
		
		ONLINE_LOG(Log, TEXT("Session: Creation of the session [Name: %s] was a success."), *SettingsSessionName)

		
		UEventBus::Broadcast<const FString&>(this, Online_Callback_OnCreateSessionCompleted,
			SettingsSessionName, TWeakPtr<const FOnlineSessionSettings>{ OnlineSessionSettings }, bWasSuccessful);
		
		if (UBROnlineSettings::Get()->bAutoLoadMapOnSessionCreation && bWasSuccessful)
		{
			const FName LoadedMapPath{ UBROnlineSettings::Get()->LoadedMapOnSessionCreation.GetLongPackageName() };
			const FString& LoadedMapOptions{ UBROnlineSettings::Get()->LoadedMapOptionsOnSessionCreation };
			UGameplayStatics::OpenLevel(this, LoadedMapPath, true, LoadedMapOptions);
		}
	}, OnlineData.CurrentOnlineSessionSettings,
	Online::GetPresenceInterface(World),
	Online::GetIdentityInterface(World),
	Online::GetSessionInterface(World));
}

void UBROnlineSubsystem::OnDestroySessionRequested(int32 LocalUserNum, FName)
{
	if (Internal_ExecuteOnValidContext(
		[this, LocalUserNum](const TSharedPtr<FOnlineSessionSettings>& OnlineSessionSettings)
		{
			FString SessionName;
			if (!OnlineSessionSettings->Get(Online_Settings_Session_Name, SessionName))
			{
				return false;
			}

			ONLINE_LOG(Log,
				TEXT("Session: The Destroying process of the session [Name: %s] has been requested."),
				*SessionName)
			
			UEventBus::Broadcast<const FString&>(this, Online_Callback_OnDestroySessionRequested, SessionName, LocalUserNum);

			return true;
		
		}, OnlineData.CurrentOnlineSessionSettings))
	{
		return;
	}

	ONLINE_LOG(Error, TEXT("Session: Unable to retrieve session settings. Fallback to the error broadcast."))
	
	UEventBus::Broadcast<const FString&>(this, Online_Callback_OnDestroySessionRequested,
		FString{ Online_Settings_Error }, LocalUserNum);
}

void UBROnlineSubsystem::OnDestroySessionCompleted(FName, bool bWasSuccessful)
{
	if (Internal_ExecuteOnValidContext(
		[this, bWasSuccessful](const TSharedPtr<FOnlineSessionSettings>& OnlineSessionSettings)
		{
			FString SessionName;
			if (!OnlineSessionSettings->Get(Online_Settings_Session_Name, SessionName))
			{
				return false;
			}

			if (!bWasSuccessful)
			{
				ONLINE_LOG(Error,
					TEXT("Session: The Destroying process of the session [Name: %s] has failed."),
					*SessionName)
			}
			else
			{
				ONLINE_LOG(Log,
					TEXT("Session: The Destroying process of the session [Name: %s] was a success."),
					*SessionName)
			}

			UEventBus::Broadcast<const FString&>(this, Online_Callback_OnDestroySessionCompleted,
				SessionName, bWasSuccessful);

			return true;

		}, OnlineData.CurrentOnlineSessionSettings))
	{
		return;
	}

	ONLINE_LOG(Error, TEXT("Session: Unable to retrieve session settings. Fallback to the error broadcast."))
	
	UEventBus::Broadcast<const FString&>(this, Online_Callback_OnDestroySessionCompleted,
		FString{ Online_Settings_Error }, bWasSuccessful);
}

void UBROnlineSubsystem::OnFindSessionsCompleted(bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		ONLINE_LOG(Error, TEXT("Session: Session search was unsuccessful due to an unknown error."))
	}
	else
	{
		Internal_ExecuteOnValidContext(
		[this, bWasSuccessful](const TSharedPtr<FOnlineSessionSearch>& OnlineSessionSearch)
		{
			const TArray<FOnlineSessionSearchResult>& SearchResults{ OnlineSessionSearch->SearchResults };
			UEventBus::Broadcast<const TArray<FOnlineSessionSearchResult>&>(
				this, Online_Callback_OnFindSessionsCompleted, SearchResults, bWasSuccessful);
		}, OnlineData.CurrentSearchedSessionSettings);

		return;
	}

	UEventBus::Broadcast<const TArray<FOnlineSessionSearchResult>&>(
		this, Online_Callback_OnFindSessionsCompleted, {}, bWasSuccessful);
}

void UBROnlineSubsystem::OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		ONLINE_LOG(Error, TEXT("Session: The join of the session has failed."))
	}
	else
	{
		Internal_ExecuteOnValidContext(
		[this, SessionName](const IOnlineSessionPtr& SessionInterface, const IOnlineExternalUIPtr& ExternalUIInterface)
		{
			FString ConnectionInfo{};
			if (SessionInterface->GetResolvedConnectString(SessionName, ConnectionInfo))
			{
				if (APlayerController* Controller{ GetWorld()->GetFirstPlayerController() })
				{
					Controller->ClientTravel(ConnectionInfo, TRAVEL_Absolute);
				}
			}
		}, Online::GetSessionInterface(GetWorld()), Online::GetExternalUIInterface(GetWorld()));
	}

	UEventBus::Broadcast(this, Online_Callback_OnJoinSessionCompleted, Result);
}

void UBROnlineSubsystem::OnSessionUserInviteAccepted(const bool bWasSuccessful, const int32 ControllerId,
                                                     FUniqueNetIdPtr UserId, const FOnlineSessionSearchResult& InviteResult)
{
	if (!bWasSuccessful || !InviteResult.IsValid())
	{
		ONLINE_LOG(Error, TEXT("Session: Accepted invitation has invalid information or was not successful."))	
	}
	else
	{
		Internal_ExecuteOnValidContext(
		[this, &InviteResult](const IOnlineSessionPtr& SessionInterface)
		{
			ONLINE_LOG(Log, TEXT("Session: Attempting to join a session made by [Player Name: %s]."), *InviteResult.Session.OwningUserName)
			
			SessionInterface->JoinSession(0, NAME_GameSession, InviteResult);
		}, Online::GetSessionInterface(GetWorld()));
	}

	UEventBus::Broadcast<const bool, const int32, FUniqueNetIdPtr, const FOnlineSessionSearchResult&>(this,
		Online_Callback_OnSessionUserInviteAccepted, bWasSuccessful, ControllerId, UserId, InviteResult);
}

void UBROnlineSubsystem::OnSessionStartCompleted(FName, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		ONLINE_LOG(Error, TEXT("Session: Start of the session was not successful."))
	}
	else
	{
		// Should activate seamless travel on the game mode.

		if (UBROnlineSettings::Get()->bAutoLoadMapOnSessionStart)
		{
			const FString MapPath{ UBROnlineSettings::Get()->LoadedMapOnSessionStart.GetLongPackageName() };
			const FString MapOptions{ UBROnlineSettings::Get()->LoadedMapOptionsOnSessionStart };
			const FString URL{ FString::Printf(TEXT("%s?%s"), *MapPath, *MapOptions) };
			
			GetWorld()->ServerTravel(URL, true);
		}
	}

	UEventBus::Broadcast(this, Online_Callback_OnSessionStartCompleted, bWasSuccessful);
}

void UBROnlineSubsystem::OnSessionInviteReceived(const FUniqueNetId& UserId, const FUniqueNetId& FromId,
	const FString& AppId, const FOnlineSessionSearchResult& InviteResult)
{
	UEventBus::Broadcast<const FUniqueNetId&, const FUniqueNetId&, const FString&, const FOnlineSessionSearchResult&>(
		this, Online_Callback_OnSessionInviteReceived, UserId, FromId, AppId, InviteResult);
}

void UBROnlineSubsystem::OnRefreshSessionTimerFinish()
{
	UEventBus::Broadcast(this, Online_Callback_OnRefreshSessionTimerFinish);
	
	FindSessions();
}

void UBROnlineSubsystem::OnPresenceReceived(const FUniqueNetId& UserId, const TSharedRef<FOnlineUserPresence>& Presence)
{
#if !UE_BUILD_SHIPPING
	Internal_ExecuteOnValidContext(
	[this, &UserId, &Presence](const IOnlineFriendsPtr& FriendsInterface)
	{
		if (!UserId.IsValid())
		{
			return;
		}
		
		if (const TSharedPtr OnlineFriend
		{
			FriendsInterface->GetFriend(0, UserId, EFriendsLists::ToString(EFriendsLists::Default))
		})
		{
			ONLINE_LOG(Log,
				TEXT("Presence: Updated user [Name: %s] presence [Presence info: %s]."),
					*OnlineFriend->GetDisplayName(), *Presence->ToDebugString())
		}
	}, Online::GetFriendsInterface(GetWorld()));
#endif
		
	UEventBus::Broadcast<const  FUniqueNetId&, const TSharedRef<FOnlineUserPresence>&>(
		this, Online_Callback_OnPresenceReceived, UserId, Presence);
}

void UBROnlineSubsystem::OnReadFriendsListComplete(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName,
                                                   const FString& ErrorStr)
{
	if (!bWasSuccessful)
	{
		ONLINE_LOG(Error, TEXT("Friends: Reading of the friends list was unsuccessful [Error: %s]"), *ErrorStr)
	}
	else
	{
		if (Internal_ExecuteOnValidContext(
		[this, LocalUserNum, bWasSuccessful, &ListName, &ErrorStr](const IOnlineFriendsPtr& FriendsInterface)
		{
			TArray<TSharedRef<FOnlineFriend>> OnlineFriends;
			if (!FriendsInterface->GetFriendsList(0, ListName, OnlineFriends))
			{
				return false;
			}
			
			UEventBus::Broadcast<int32, bool, const TArray<TSharedRef<FOnlineFriend>>&, const FString&>(this,
				Online_Callback_OnReadFriendsListCompleted, LocalUserNum, bWasSuccessful, OnlineFriends, ErrorStr);

			return true;
		}, Online::GetFriendsInterface(GetWorld())))
		{
			return;
		}

		ONLINE_LOG(Error,
			TEXT("Friends: Unable to retrieve the Friends interface or could not get the friends list."))
	}
	
	UEventBus::Broadcast<int32, bool, const TArray<TSharedRef<FOnlineFriend>>&, const FString&>(this,
	Online_Callback_OnReadFriendsListCompleted, LocalUserNum, bWasSuccessful, {}, ErrorStr);
}

void UBROnlineSubsystem::OnAvatarTextureRetrieved(UTexture2DDynamic* Texture, FUniqueNetIdWeakPtr UserId)
{
	if (!Texture)
	{
		ONLINE_LOG(Error, TEXT("Friends: The avatar texture was not retrieved correctly."))
	}
	
	UEventBus::Broadcast(this, Online_Callback_OnAvatarTextureRetrieved, Texture, UserId);
}

void UBROnlineSubsystem::OnBlockListChange(int32 LocalUserNum, const FString& ListName)
{
	QueryFriendList();
}

void UBROnlineSubsystem::OnFriendRemoved(const FUniqueNetId& UserId, const FUniqueNetId& FriendId)
{
	QueryFriendList();
}

void UBROnlineSubsystem::OnInviteAccepted(const FUniqueNetId& UserId, const FUniqueNetId& FriendId)
{
	QueryFriendList();
}

void UBROnlineSubsystem::OnExternalOverlayOpen(const EOS_UI_OnDisplaySettingsUpdatedCallbackInfo* Data)
{
	UBROnlineSubsystem* This = static_cast<UBROnlineSubsystem*>(Data->ClientData);

	
	const bool bOverlayVisible = Data->bIsVisible == EOS_TRUE;
	const bool bHasExclusiveInput = Data->bIsExclusiveInput == EOS_TRUE;
	
	if (bOverlayVisible && bHasExclusiveInput && !This->bMainExternalOverlayIsOpen)
	{ 
		This->bMainExternalOverlayIsOpen = true;
		UEventBus::Broadcast(This->GetWorld(), Online_Callback_OnMainExternalUIOverlayChange, true);
		return;
	}
	if (!bOverlayVisible && This->bMainExternalOverlayIsOpen)
	{
		This->bMainExternalOverlayIsOpen = false;
		UEventBus::Broadcast(This->GetWorld(), Online_Callback_OnMainExternalUIOverlayChange, false);
		return;
	}
	
}

EOS_HUI UBROnlineSubsystem::GetExternalUIHandle()
{
	IEOSSDKManager* SDKManager = IEOSSDKManager::Get();
	if (!SDKManager || SDKManager->GetActivePlatforms().IsEmpty())
		return nullptr;

	return EOS_Platform_GetUIInterface(*SDKManager->GetActivePlatforms()[0]);
}

void UBROnlineSubsystem::UpdatePresence(EOnlinePresenceState::Type State, const FString& StatusText, bool bIsJoinable) const
{
	const UWorld* World{ GetWorld() };

	return Internal_ExecuteOnValidContext(
	[this, State, &StatusText, bIsJoinable]
	(const IOnlinePresencePtr& PresenceInterface, const IOnlineIdentityPtr& IdentityInterface)
	{
		const FUniqueNetIdPtr UniquePlayerId{ IdentityInterface->GetUniquePlayerId(0) };
		if (!UniquePlayerId || !UniquePlayerId->IsValid())
		{
			ONLINE_LOG(Error, TEXT("Presence: Cannot update presence, local player id is invalid."))
		}

		FOnlineUserPresenceStatus NewStatus;
		NewStatus.State  = State;
		NewStatus.StatusStr = StatusText;
		NewStatus.Properties.Add(TEXT("Joinable"), bIsJoinable);

		PresenceInterface->SetPresence(*UniquePlayerId, NewStatus,
			IOnlinePresence::FOnPresenceTaskCompleteDelegate::CreateLambda(
			[this](const FUniqueNetId& UserId, const bool bWasSuccessful)
			{
				if (!bWasSuccessful)
				{
					ONLINE_LOG(Error, TEXT("Presence: Failed to update presence for user [%s]"), *UserId.ToString())
				}
			}));

	}, Online::GetPresenceInterface(World), Online::GetIdentityInterface(World));
}

int UBROnlineSubsystem::GetMaxPlayerCountSession() const
{
	const UWorld* World{GetWorld()};

	return Internal_ExecuteOnValidContext(
		[this](const IOnlineSessionPtr& SessionInterface)
	{
			return Internal_ExecuteOnValidContext([this] (const FNamedOnlineSession* CurrentSession)
			{
				return CurrentSession->SessionSettings.NumPrivateConnections + CurrentSession->SessionSettings.NumPublicConnections;
			},
			SessionInterface->GetNamedSession(NAME_GameSession));
	}, Online::GetSessionInterface(World));
}

int UBROnlineSubsystem::GetCurrentPlayerCountSession() const
{
	const UWorld* World{GetWorld()};
	
	return Internal_ExecuteOnValidContext(
		[this](const IOnlineSessionPtr& SessionInterface)
	{
			return Internal_ExecuteOnValidContext([this] (const FNamedOnlineSession* CurrentSession)
			{
				return FMath::Abs(CurrentSession->NumOpenPrivateConnections + CurrentSession->NumOpenPublicConnections - GetMaxPlayerCountSession());
			},
			SessionInterface->GetNamedSession(NAME_GameSession));
	}, Online::GetSessionInterface(World));
}

bool UBROnlineSubsystem::PlayerIsInSession() const
{
	const UWorld* World{ GetWorld() };
	
	return Internal_ExecuteOnValidContext(
		[this](const IOnlineSessionPtr& SessionInterface)
	{
		if (const FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession))
			{
				const EOnlineSessionState::Type State = ExistingSession->SessionState;

				if (State == EOnlineSessionState::InProgress || 
					State == EOnlineSessionState::Pending || 
					State == EOnlineSessionState::Starting)
				{
					return true;
				}
			}
			return false;
	}, Online::GetSessionInterface(World));
}

void UBROnlineSubsystem::Internal_RegisterDelegates()
{
	const UWorld* World{ GetWorld() };
	
	Internal_ExecuteOnValidContext(
	[this](const IOnlineIdentityPtr& IdentityInterface)
	{
		IdentityInterface->AddOnLoginCompleteDelegate_Handle(0,
			FOnLoginCompleteDelegate::CreateUObject(this, &UBROnlineSubsystem::OnLoggingCompleted));

		IdentityInterface->AddOnLogoutCompleteDelegate_Handle(0,
			FOnLogoutCompleteDelegate::CreateUObject(this, &UBROnlineSubsystem::OnLogoutCompleted));

		IdentityInterface->AddOnLoginStatusChangedDelegate_Handle(0,
			FOnLoginStatusChangedDelegate::CreateUObject(this, &UBROnlineSubsystem::OnLoginStatusChanged));
	}, Online::GetIdentityInterface(World));

	Internal_ExecuteOnValidContext(
	[this](const IOnlineSessionPtr& SessionInterface)
	{
		SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(
			FOnCreateSessionCompleteDelegate::CreateUObject(this, &UBROnlineSubsystem::OnCreateSessionCompleted));
	
		SessionInterface->AddOnDestroySessionRequestedDelegate_Handle(
			FOnDestroySessionRequestedDelegate::CreateUObject(this, &UBROnlineSubsystem::OnDestroySessionRequested));
	
		SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(
			FOnDestroySessionCompleteDelegate::CreateUObject(this, &UBROnlineSubsystem::OnDestroySessionCompleted));

		SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(
			FOnFindSessionsCompleteDelegate::CreateUObject(this, &UBROnlineSubsystem::OnFindSessionsCompleted));
		
		SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(
			FOnJoinSessionCompleteDelegate::CreateUObject(this, &UBROnlineSubsystem::OnJoinSessionCompleted));
		
		SessionInterface->AddOnSessionUserInviteAcceptedDelegate_Handle(
			FOnSessionUserInviteAcceptedDelegate::CreateUObject(this, &UBROnlineSubsystem::OnSessionUserInviteAccepted));

		SessionInterface->AddOnStartSessionCompleteDelegate_Handle(
			FOnStartSessionCompleteDelegate::CreateUObject(this, &UBROnlineSubsystem::OnSessionStartCompleted));

		SessionInterface->AddOnSessionInviteReceivedDelegate_Handle(
			FOnSessionInviteReceivedDelegate::CreateUObject(this, &UBROnlineSubsystem::OnSessionInviteReceived));

	}, Online::GetSessionInterface(World));

	Internal_ExecuteOnValidContext([this](const IOnlinePresencePtr& PresenceInterface)
	{
		PresenceInterface->AddOnPresenceReceivedDelegate_Handle(
			FOnPresenceReceivedDelegate::CreateUObject(this, &UBROnlineSubsystem::OnPresenceReceived));
	}, Online::GetPresenceInterface(World));

	Internal_ExecuteOnValidContext([this](const IOnlineFriendsPtr& FriendsInterface)
	{
		FriendsInterface->AddOnBlockListChangeDelegate_Handle(0, 
			FOnBlockListChangeDelegate::CreateUObject(this, &UBROnlineSubsystem::OnBlockListChange));
		
		FriendsInterface->AddOnFriendRemovedDelegate_Handle(
			FOnFriendRemovedDelegate::CreateUObject(this, &UBROnlineSubsystem::OnFriendRemoved));

		FriendsInterface->AddOnInviteAcceptedDelegate_Handle(
			FOnInviteAcceptedDelegate::CreateUObject(this, &UBROnlineSubsystem::OnInviteAccepted));
	}, Online::GetFriendsInterface(World));
	
	
	EOS_HUI UIHandle = GetExternalUIHandle();
	if (!UIHandle) return;

	EOS_UI_AddNotifyDisplaySettingsUpdatedOptions Options = {};
	Options.ApiVersion = EOS_UI_ADDNOTIFYDISPLAYSETTINGSUPDATED_API_LATEST;

	OverlayNotifId = EOS_UI_AddNotifyDisplaySettingsUpdated(
		UIHandle,
		&Options,
		this,
		&UBROnlineSubsystem::OnExternalOverlayOpen
	);
}

void UBROnlineSubsystem::Internal_ClearDelegates()
{
	const UWorld* World{ GetWorld() };
	
	Internal_ExecuteOnValidContext(
	[this](const IOnlineIdentityPtr& IdentityInterface)
	{
		IdentityInterface->ClearOnLoginCompleteDelegates(0, this);
		IdentityInterface->ClearOnLogoutCompleteDelegates(0, this);
		IdentityInterface->ClearOnLoginStatusChangedDelegates(0, this);
	}, Online::GetIdentityInterface(World));

	Internal_ExecuteOnValidContext(
	[this](const IOnlineSessionPtr& SessionInterface)
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegates(this);
		SessionInterface->ClearOnDestroySessionRequestedDelegates(this);
		SessionInterface->ClearOnDestroySessionCompleteDelegates(this);
		SessionInterface->ClearOnFindSessionsCompleteDelegates(this);
		SessionInterface->ClearOnSessionUserInviteAcceptedDelegates(this);
		SessionInterface->ClearOnStartSessionCompleteDelegates(this);
		SessionInterface->ClearOnSessionInviteReceivedDelegates(this);
	}, Online::GetSessionInterface(World));

	Internal_ExecuteOnValidContext([this](const IOnlinePresencePtr& PresenceInterface)
	{
		PresenceInterface->ClearOnPresenceReceivedDelegates(this);
	}, Online::GetPresenceInterface(World));

	Internal_ExecuteOnValidContext([this](const IOnlineFriendsPtr& FriendsInterface)
	{
		FriendsInterface->ClearOnFriendsChangeDelegates(0, this);
	}, Online::GetFriendsInterface(World));
	
	if (OverlayNotifId != EOS_INVALID_NOTIFICATIONID)
	{
		EOS_HUI UIHandle = GetExternalUIHandle();
		if (UIHandle)
		{
			EOS_UI_RemoveNotifyDisplaySettingsUpdated(UIHandle, OverlayNotifId);
			OverlayNotifId = EOS_INVALID_NOTIFICATIONID;
		}
	}

}

void UBROnlineSubsystem::Internal_LockCallbacksSignature()
{
	UEventBus::LockSignature<int32, bool, const FUniqueNetId&, const FString&>(this, Online_Callback_OnLoginComplete);
	UEventBus::LockSignature<int32, bool>(this, Online_Callback_OnLogoutComplete);
	UEventBus::LockSignature<int32, ELoginStatus::Type, ELoginStatus::Type, const FUniqueNetId&>(this, Online_Callback_OnLoginStatusChanged);

	UEventBus::LockSignature<const FString&, TWeakPtr<const FOnlineSessionSettings>, bool>(this, Online_Callback_OnCreateSessionCompleted);
	UEventBus::LockSignature<const FString&, int32>(this, Online_Callback_OnDestroySessionRequested);
	UEventBus::LockSignature<const FString&, bool>(this, Online_Callback_OnDestroySessionCompleted);
	UEventBus::LockSignature<const TArray<FOnlineSessionSearchResult>&, bool>(this, Online_Callback_OnFindSessionsCompleted);
	UEventBus::LockSignature<EOnJoinSessionCompleteResult::Type>(this, Online_Callback_OnJoinSessionCompleted);
	UEventBus::LockSignature<const bool, const int32, FUniqueNetIdPtr, const FOnlineSessionSearchResult&>(this, Online_Callback_OnSessionUserInviteAccepted);
	UEventBus::LockSignature<const FUniqueNetId&, const FUniqueNetId&, const FString&, const FOnlineSessionSearchResult&>(this, Online_Callback_OnSessionInviteReceived);
	
	UEventBus::LockSignature<const FUniqueNetId&, const TSharedRef<FOnlineUserPresence>&>(this, Online_Callback_OnPresenceReceived);
	UEventBus::LockSignature<int32, bool, const TArray<TSharedRef<FOnlineFriend>>&, const FString&>(this, Online_Callback_OnReadFriendsListCompleted);
	UEventBus::LockSignature<UTexture2DDynamic*, FUniqueNetIdWeakPtr>(this, Online_Callback_OnAvatarTextureRetrieved);
	
	UEventBus::LockSignature<bool>(this, Online_Callback_OnMainExternalUIOverlayChange);
}
