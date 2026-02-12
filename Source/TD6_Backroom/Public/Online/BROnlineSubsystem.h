// Copyright (c) Logicraft Interactive. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TimerHolder.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "BROnlineSubsystem.generated.h"

constexpr int32 MaxSessionConnections{ 4 };

constexpr auto Online_Settings_Session_Name{ TEXT("SETTINGS_SESSION_NAME") };
constexpr auto Online_Settings_Error{ TEXT("Session Settings Error") };

struct FOnlineSubsystemData
{
	TSharedPtr<FOnlineSessionSettings> CurrentOnlineSessionSettings;
	TSharedPtr<FOnlineSessionSearch> CurrentSearchedSessionSettings;

	FTimerHolder RefreshSessionsTimer;
};

/**
 * TODO :
 * 1. Don't forget about the friends request that could update the friends list.
 * 2. Make a good join function.
 * 3. Change Online parameters with session creation.
 */

/**
 * 
 */
UCLASS()
class TD6_BACKROOM_API UBROnlineSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

private:
	FOnlineSubsystemData OnlineData;
	
public:
	static ThisClass* Get(const UObject* WorldContext);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	bool Login(bool bUseCommandLine = true,
				const FOnlineAccountCredentials& OnlineAccountCredentials = FOnlineAccountCredentials{
					"AccountPortal", "", ""
				}) const;
	bool Logout();

	bool CreateSession(int32 SessionMaxConnections = MaxSessionConnections, bool bIsPrivate = false);
	bool DestroySession();
	bool FindSessions();
	bool JoinSession(const FOnlineSessionSearchResult& DesiredSession);

	void LaunchRefreshSessionsTimer(float Rate);
	void StopRefreshSessionTimer();
	
	bool QueryFriendList();
	FString GetAvatarURL(TSharedRef<FOnlineUser> OnlineUser);
	void RetrievedAvatarTexture(const FString& AvatarUrl, FUniqueNetIdRef UserId);
	
private:
	void OnLoggingCompleted(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);
	void OnLogoutCompleted(int32 LocalUserNum, bool bWasSuccessful);
	void OnLoginStatusChanged(int32 LocalUserNum, ELoginStatus::Type OldStatus, ELoginStatus::Type NewStatus, const FUniqueNetId& NewId);

	void OnCreateSessionCompleted(FName, bool bWasSuccessful);
	void OnDestroySessionRequested(int32 LocalUserNum, FName);
	void OnDestroySessionCompleted(FName, bool bWasSuccessful);
	void OnFindSessionsCompleted(bool bWasSuccessful);
	void OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnSessionUserInviteAccepted(const bool bWasSuccessful, const int32 ControllerId, FUniqueNetIdPtr UserId, const FOnlineSessionSearchResult& InviteResult);
	void OnSessionStartCompleted(FName, bool bWasSuccessful);
	// void OnSessionParticipantJoined(FName SessionName, const FUniqueNetId& UserId);

	void OnRefreshSessionTimerFinish();

	void OnPresenceReceived(const FUniqueNetId& UserId, const TSharedRef<FOnlineUserPresence>& Presence);
	void OnReadFriendsListComplete(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr);
	void OnAvatarTextureRetrieved(UTexture2DDynamic* Texture, FUniqueNetIdWeakPtr UserId);
	
	void Internal_RegisterDelegates();
	void Internal_ClearDelegates();

	void Internal_LockCallbacksSignature();

	template <typename TFunctor, typename ...TContext>
	static auto Internal_ExecuteOnValidContext(TFunctor&& Functor, TContext... Context)
	{
		using ReturnType = std::invoke_result_t<TFunctor, TContext...>;
		
		if ((Context && ...))
		{
			return Functor(Context...);
		}

		if constexpr (!std::is_void_v<ReturnType>)
		{
			return ReturnType{};	
		}
	}
};
