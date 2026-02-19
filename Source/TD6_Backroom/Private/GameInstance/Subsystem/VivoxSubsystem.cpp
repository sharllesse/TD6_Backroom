// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstance/Subsystem/VivoxSubsystem.h"

#include "Chain.h"
#include "VivoxCore.h"
#include "Online/BROnlineSubsystem.h"
#include "VivoxCore/Public/IClient.h"

DEFINE_LOG_CATEGORY(Log_BRVivox);

void UVivoxSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	VoiceModule = static_cast<FVivoxCoreModule*>(&FModuleManager::Get().LoadModuleChecked(TEXT("VivoxCore")));
	if (!VoiceModule)
	{
		UE_LOG(Log_BRVivox, Error, TEXT("Failed to get VivoxCore module!"));
		return;
	}
	
	VoiceClient = &VoiceModule->VoiceClient();
	VoiceClient->Initialize();
	
}

void UVivoxSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UVivoxSubsystem::Login()
{
	TOptional<FString> UserName = Chain::StartChain(UBROnlineSubsystem::Get(GetWorld()))
	.GetValue(&UBROnlineSubsystem::GetLocalPlayerUserName);
	
	if (!VoiceClient)
	{
		UE_LOG(Log_BRVivox, Error, TEXT("Unable to retrieve voice client, can't login to vivox!"));
		return;
	}
	
	if (!UserName)
	{
		UE_LOG(Log_BRVivox, Error, TEXT("Unable to retrieve local player username, can't login to vivox!"));
		return;
	}
	
	AccountId Account = AccountId(TokenIssuer, *UserName, Domain);
	ILoginSession &NewLoginSession(VoiceClient->GetLoginSession(Account));
	ILoginSession::FOnBeginLoginCompletedDelegate OnBeginLoginCompleted;
	OnBeginLoginCompleted.BindLambda([this, &NewLoginSession](VivoxCoreError Result)
	{
		if (VxErrorSuccess == Result)
		{
			bIsLoggedIn = true; 
			LoginSession = &NewLoginSession;
		}
	});
	
	NewLoginSession.EventStateChanged.AddLambda([](LoginState State)
	{
		if (State == LoginState::LoggedIn)
		{
			UE_LOG(Log_BRVivox, Log, TEXT("Player successfully logged into Vivox."));
		}
		else
		{
			UE_LOG(Log_BRVivox, Log, TEXT("Player disconnected from Vivox."));
		}
	});
	
	NewLoginSession.BeginLogin("https://unity.vivox.com/appconfig/90721-td6_b-93439", 
		NewLoginSession.GetLoginToken(TokenKey, FTimespan::FromDays(1)), OnBeginLoginCompleted);

}

void UVivoxSubsystem::Logout()
{
}
