// Copyright (c) Logicraft Interactive. All Rights Reserved.

#include "TD6_Backroom/Public/Online/BROnlineSubsystem.h"

#include "EventBus.h"
#include "LogCategory.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#include "Online/BROnlineGameTags.h"
#include "Online/BROnlineSettings.h"

#include "Interfaces/OnlineExternalUIInterface.h"
#include "Kismet/GameplayStatics.h"

#define ONLINE_LOG(Verbosity, Format, ...)\
	UE_LOG(LogOWOnline, Verbosity, Format, ##__VA_ARGS__)

UOWOnlineSubsystem::ThisClass* UOWOnlineSubsystem::Get(const UObject* WorldContext)
{
	if (UWorld* World{GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull)})
	{
		return World->GetGameInstance()->GetSubsystem<ThisClass>();
	}

	return nullptr;
}

void UOWOnlineSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	Collection.InitializeDependency<UEventBus>();

	Internal_RegisterDelegates();
	Internal_LockCallbacksSignature();
	
	if (UOWOnlineSettings::Get()->bAutoLogin)
	{
		Login();
	}
}

void UOWOnlineSubsystem::Deinitialize()
{
	Super::Deinitialize();

	Internal_ClearDelegates();
}

bool UOWOnlineSubsystem::Login(bool bUseCommandLine, const FOnlineAccountCredentials& OnlineAccountCredentials) const
{
	return Internal_ExecuteOnValidContext(
	[bUseCommandLine, &OnlineAccountCredentials](const IOnlineIdentityPtr& IdentityInterface)
	{
		if (bUseCommandLine)
		{
			FString AuthType;
			FParse::Value(FCommandLine::Get(), TEXT("AUTH_TYPE="), AuthType);
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

bool UOWOnlineSubsystem::Logout()
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

bool UOWOnlineSubsystem::CreateSession(int32 SessionMaxConnections, bool bIsPrivate)
{
	UWorld* World{ GetWorld() };
	
	return Internal_ExecuteOnValidContext(
	[this, SessionMaxConnections, bIsPrivate]
	(const IOnlineSessionPtr& SessionInterface, const IOnlineIdentityPtr& IdentityInterface) mutable 
	{
		const FUniqueNetIdPtr UniquePlayerId{ IdentityInterface->GetUniquePlayerId(0) };
		const FString SessionName{ FString::Printf(TEXT("%s_Session"), *UniquePlayerId->ToString()) };

		FOnlineSessionSettings OnlineSessionSettings;
		OnlineSessionSettings.bAllowJoinInProgress = true;
		OnlineSessionSettings.bAllowJoinViaPresenceFriendsOnly = !bIsPrivate;
		OnlineSessionSettings.bAllowInvites = true;
		OnlineSessionSettings.bUsesPresence = true;
		OnlineSessionSettings.bUsesStats = true;
		OnlineSessionSettings.Set("SETTINGS_SESSION_NAME", SessionName, EOnlineDataAdvertisementType::ViaOnlineService);

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
		
		return SessionInterface->CreateSession(*UniquePlayerId, NAME_GameSession, OnlineSessionSettings);
	}, Online::GetSessionInterface(World), Online::GetIdentityInterface(World));
}

void UOWOnlineSubsystem::OnLoggingCompleted(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId,
                                            const FString& Error)
{
	if (!bWasSuccessful)
	{
		ONLINE_LOG(Error, TEXT("Login: Login was unsuccessful due to a error [Error: %s]"), *Error)
	}
#if !UE_BUILD_SHIPPING
	else
	{
		Internal_ExecuteOnValidContext(
		[&UserId](const IOnlineIdentityPtr& IdentityInterface)
		{
			ONLINE_LOG(Log, TEXT("Login: Login was successful [User Name: %s]"),
						*IdentityInterface->GetPlayerNickname(UserId))
		}, Online::GetIdentityInterface(GetWorld()));	
	}
#endif
	
	UEventBus::Broadcast<int32, bool, const FUniqueNetId&, const FString&>
	(this, Online_Callback_OnLoginComplete, LocalUserNum, bWasSuccessful, UserId, Error);
}

void UOWOnlineSubsystem::OnLogoutCompleted(int32 LocalUserNum, bool bWasSuccessful)
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

void UOWOnlineSubsystem::OnLoginStatusChanged(int32 LocalUserNum, ELoginStatus::Type OldStatus,
	ELoginStatus::Type NewStatus, const FUniqueNetId& NewId)
{
	ONLINE_LOG(Log,
		TEXT("Login: Login status has changed [OldStatus: %s, NewStatus: %s]."),
		ELoginStatus::ToString(OldStatus), ELoginStatus::ToString(NewStatus))
	
	UEventBus::Broadcast<int32, ELoginStatus::Type, ELoginStatus::Type, const FUniqueNetId&>
	(this, Online_Callback_OnLoginStatusChanged, LocalUserNum, OldStatus, NewStatus, NewId);
}

void UOWOnlineSubsystem::OnCreateSessionCompleted(FName SessionName, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		ONLINE_LOG(Error, TEXT("Session: Creation of the session [Name: %s] has failed."), *SessionName.ToString())
	}
	else
	{
		ONLINE_LOG(Log, TEXT("Session: Creation of the session [Name: %s] was a success."), *SessionName.ToString())
	}
	
	UEventBus::Broadcast(this, Online_Callback_OnCreateSessionCompleted, SessionName, bWasSuccessful);

	UGameplayStatics::OpenLevel(this, "OnlineTestingMap", true, "listen");
}

void UOWOnlineSubsystem::OnDestroySessionRequested(int32 LocalUserNum, FName SessionName)
{
	ONLINE_LOG(Log, TEXT("Session: The Destroying process of the session [Name: %s] has benn requested."), *SessionName.ToString())
	
	UEventBus::Broadcast(this, Online_Callback_OnDestroySessionRequested, LocalUserNum, SessionName);
}

void UOWOnlineSubsystem::OnDestroySessionCompleted(FName SessionName, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		ONLINE_LOG(Error, TEXT("Session: The Destroying process of the session [Name: %s] has failed."), *SessionName.ToString())
	}
	else
	{
		ONLINE_LOG(Log, TEXT("Session: The Destroying process of the session [Name: %s] was a success."), *SessionName.ToString())
	}
	
	UEventBus::Broadcast(this, Online_Callback_OnDestroySessionCompleted, SessionName, bWasSuccessful);
}

void UOWOnlineSubsystem::OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
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

	UEventBus::Broadcast(this, Online_Callback_OnJoinSessionCompleted, SessionName, Result);
}

void UOWOnlineSubsystem::OnSessionUserInviteAccepted(const bool bWasSuccessful, const int32 ControllerId,
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
			const FUniqueNetIdPtr UniquePlayerId{ InviteResult.Session.OwningUserId };
			const FString SessionName{ FString::Printf(TEXT("%s_Session"), *UniquePlayerId->ToString()) };

			ONLINE_LOG(Log, TEXT("Session: Attempting to join a session [Name: %s] made by [Player Name: %s]."), *SessionName, *InviteResult.Session.OwningUserName)
			
			SessionInterface->JoinSession(0, FName{ SessionName }, InviteResult);
		}, Online::GetSessionInterface(GetWorld()));	
	}
}

void UOWOnlineSubsystem::OnSessionParticipantJoined(FName SessionName, const FUniqueNetId& UserId)
{
	Internal_ExecuteOnValidContext(
	[this, &UserId](const IOnlineIdentityPtr& IdentityInterface)
	{
		const FString PlayerNickname{ IdentityInterface->GetPlayerNickname(UserId) };

		ONLINE_LOG(Log, TEXT("Session: A player has joined the session [Player Name: %s]."), *PlayerNickname)	
	}, Online::GetIdentityInterface(GetWorld()));
}

void UOWOnlineSubsystem::OnExternalUIChange(bool bIsOpening)
{
	UEventBus::Broadcast(this, Online_Callback_OnExternalUIChange, bIsOpening);
}

void UOWOnlineSubsystem::Internal_RegisterDelegates()
{
	Internal_ExecuteOnValidContext(
	[this](const IOnlineIdentityPtr& IdentityInterface)
	{
		IdentityInterface->AddOnLoginCompleteDelegate_Handle(0,
			FOnLoginCompleteDelegate::CreateUObject(this, &UOWOnlineSubsystem::OnLoggingCompleted));

		IdentityInterface->AddOnLogoutCompleteDelegate_Handle(0,
			FOnLogoutCompleteDelegate::CreateUObject(this, &UOWOnlineSubsystem::OnLogoutCompleted));

		IdentityInterface->AddOnLoginStatusChangedDelegate_Handle(0,
			FOnLoginStatusChangedDelegate::CreateUObject(this, &UOWOnlineSubsystem::OnLoginStatusChanged));
	}, Online::GetIdentityInterface(GetWorld()));

	Internal_ExecuteOnValidContext(
	[this](const IOnlineSessionPtr& SessionInterface)
	{
		SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(
			FOnCreateSessionCompleteDelegate::CreateUObject(this, &UOWOnlineSubsystem::OnCreateSessionCompleted));

		SessionInterface->AddOnDestroySessionRequestedDelegate_Handle(
			FOnDestroySessionRequestedDelegate::CreateUObject(this, &UOWOnlineSubsystem::OnDestroySessionRequested));

		SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(
			FOnDestroySessionCompleteDelegate::CreateUObject(this, &UOWOnlineSubsystem::OnDestroySessionCompleted));

		SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(
			FOnJoinSessionCompleteDelegate::CreateUObject(this, &UOWOnlineSubsystem::OnJoinSessionCompleted));
		
		SessionInterface->AddOnSessionUserInviteAcceptedDelegate_Handle(
			FOnSessionUserInviteAcceptedDelegate::CreateUObject(this, &UOWOnlineSubsystem::OnSessionUserInviteAccepted));

		SessionInterface->AddOnSessionParticipantJoinedDelegate_Handle(
			FOnSessionParticipantJoinedDelegate::CreateUObject(this, &UOWOnlineSubsystem::OnSessionParticipantJoined));
	}, Online::GetSessionInterface(GetWorld()));

	Internal_ExecuteOnValidContext([this](const IOnlineExternalUIPtr& ExternalUIInterface)
	{
		ExternalUIInterface->AddOnExternalUIChangeDelegate_Handle(
			FOnExternalUIChangeDelegate::CreateUObject(this, &UOWOnlineSubsystem::OnExternalUIChange));
	}, Online::GetExternalUIInterface(GetWorld()));
}

void UOWOnlineSubsystem::Internal_ClearDelegates()
{
	Internal_ExecuteOnValidContext(
	[this](const IOnlineIdentityPtr& IdentityInterface)
	{
		IdentityInterface->ClearOnLoginCompleteDelegates(0, this);
		IdentityInterface->ClearOnLogoutCompleteDelegates(0, this);
		IdentityInterface->ClearOnLoginStatusChangedDelegates(0, this);
	}, Online::GetIdentityInterface(GetWorld()));

	Internal_ExecuteOnValidContext(
	[this](const IOnlineSessionPtr& SessionInterface)
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegates(this);
		SessionInterface->ClearOnDestroySessionRequestedDelegates(this);
		SessionInterface->ClearOnDestroySessionCompleteDelegates(this);
		SessionInterface->ClearOnSessionUserInviteAcceptedDelegates(this);
		SessionInterface->ClearOnSessionParticipantJoinedDelegates(this);
	}, Online::GetSessionInterface(GetWorld()));
	
	Internal_ExecuteOnValidContext([this](const IOnlineExternalUIPtr& ExternalUIInterface)
	{
		ExternalUIInterface->ClearOnExternalUIChangeDelegates(this);
	}, Online::GetExternalUIInterface(GetWorld()));
}

void UOWOnlineSubsystem::Internal_LockCallbacksSignature()
{
	UEventBus::LockSignature<int32, bool, const FUniqueNetId&, const FString&>(this, Online_Callback_OnLoginComplete);
	UEventBus::LockSignature<int32, bool>(this, Online_Callback_OnLogoutComplete);
	UEventBus::LockSignature<int32, ELoginStatus::Type, ELoginStatus::Type, const FUniqueNetId&>(this, Online_Callback_OnLoginStatusChanged);

	UEventBus::LockSignature<FName, bool>(this, Online_Callback_OnCreateSessionCompleted);
	UEventBus::LockSignature<int32, FName>(this, Online_Callback_OnDestroySessionRequested);
	UEventBus::LockSignature<FName, bool>(this, Online_Callback_OnDestroySessionCompleted);
	UEventBus::LockSignature<FName, EOnJoinSessionCompleteResult::Type>(this, Online_Callback_OnJoinSessionCompleted);

	UEventBus::LockSignature<bool>(this, Online_Callback_OnExternalUIChange);
}
