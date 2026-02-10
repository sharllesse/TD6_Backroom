// Copyright (c) Logicraft Interactive. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "BROnlineSubsystem.generated.h"

constexpr int32 MaxSessionConnections{ 4 };

/**
 * 
 */
UCLASS()
class OVERWRITE_API UOWOnlineSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

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
	
private:
	void OnLoggingCompleted(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);
	void OnLogoutCompleted(int32 LocalUserNum, bool bWasSuccessful);
	void OnLoginStatusChanged(int32 LocalUserNum, ELoginStatus::Type OldStatus, ELoginStatus::Type NewStatus, const FUniqueNetId& NewId);

	void OnCreateSessionCompleted(FName SessionName, bool bWasSuccessful);
	void OnDestroySessionRequested(int32 LocalUserNum, FName SessionName);
	void OnDestroySessionCompleted(FName SessionName, bool bWasSuccessful);
	void OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnSessionUserInviteAccepted(const bool bWasSuccessful, const int32 ControllerId, FUniqueNetIdPtr UserId, const FOnlineSessionSearchResult& InviteResult);
	void OnSessionParticipantJoined(FName SessionName, const FUniqueNetId& UserId);
	
	void OnExternalUIChange(bool bIsOpening);
	
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
