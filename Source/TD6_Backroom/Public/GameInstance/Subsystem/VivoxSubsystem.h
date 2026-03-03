// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChannelId.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "VivoxSubsystem.generated.h"

class IVoiceChatUser;
class IChannelSession;
class ILoginSession;
class IClient;
DECLARE_LOG_CATEGORY_EXTERN(Log_BRVivox, Log, Log);

class FVivoxCoreModule;
/**
 * 
 */
UCLASS()
class TD6_BACKROOM_API UVivoxSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
protected:
	FVivoxCoreModule* VoiceModule{nullptr};
	IClient* VoiceClient{nullptr};
	ILoginSession* LoginSession{nullptr};
	IChannelSession* InGameChannelSession{nullptr};
	ChannelId InGameChannelID;
	IChannelSession* LobbyChannelSession{nullptr};
	ChannelId LobbyChannelID;
	IVoiceChatUser* VivoxUser;
	
	bool bIsLoggedIn{false};
	bool bIsInVocalRoom{false};
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	inline static const FString Server{TEXT("https://unity.vivox.com/appconfig/90721-td6_b-93439")};
	inline static const FString Domain{TEXT("mtu1xp.vivox.com")};
	inline static const FString TokenIssuer{TEXT("90721-td6_b-93439")};
	inline static const FString TokenKey{TEXT("wCWOKUvKtyeOCogmHl8DPvhhSYghVEXQ")};

	void RetrieveVivoxUser();
	
	FDelegateHandle OnSessionDisconnection;
	
	FString FormatChannelSessionName(const FString& InputString);
	
	void Join3DVocalRoom(const FString& SessionName);
	void JoinLobbyVocalRoom(const FString& SessionName);
public:
	
	void Login();
	void Logout();
	
	void JoinVocalRoom();
	void LeaveVocalRoom();
	
	void SwitchTo3DRoom();
	void SwitchTo2DRoom();
	
	void Set3DPosition(const FVector& Position, const FVector& Forward, const FVector& Up);
	
	TArray<FString> GetAllMicrophones() const;
	void SetMicrophone(const FString& NewDevice) const;
	
	TArray<FString> GetAllListeners() const;
	void SetListener(const FString& NewDevice) const;
};
