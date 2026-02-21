// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstance/Subsystem/VivoxSubsystem.h"

#include "Chain.h"
#include "EventBus.h"
#include "IOnlineSubsystemEOS.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "VivoxCore.h"
#include "Character/BRCharacterGameplayTags.h"
#include "GameInstance/BRGameInstanceGameplayTags.h"
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
	
	UEventBus::LockSignature<const LoginState&>(this, GameInstance_Callback_OnVivoxLoginSessionStateChange);
	UEventBus::LockSignature<const IChannelConnectionState&>(this, GameInstance_Callback_OnVivoxChannelSessionStateChange);
}

void UVivoxSubsystem::Deinitialize()
{
	Super::Deinitialize();
	UEventBus::UnlockSignature(this, GameInstance_Callback_OnVivoxLoginSessionStateChange);
	UEventBus::UnlockSignature(this, GameInstance_Callback_OnVivoxChannelSessionStateChange);
}

void UVivoxSubsystem::RetrieveVivoxUser()
{
	auto OnlineSubsystem = Online::GetSubsystem(GetWorld());
	if (!OnlineSubsystem)
	{
		UE_LOG(Log_BRVivox, Error, TEXT("Unable to retrieve online subsystem!"));
		return;
	}
	
	if (auto VoiceChatInterface = OnlineSubsystem->GetVoiceInterface())
	{
		VoiceChatInterface.Get()->Init();
	}
}

FString UVivoxSubsystem::FormatChannelSessionName(const FString& InputString)
{
	FString SanitizedString = InputString;

	if (SanitizedString.Len() > 200)
	{
		SanitizedString = SanitizedString.Left(200);
	}

	for (int32 Index = 0; Index < SanitizedString.Len(); ++Index)
	{
		TCHAR C = SanitizedString[Index];

		bool bIsValid = (C >= '0' && C <= '9') ||
						(C >= 'A' && C <= 'Z') ||
						(C >= 'a' && C <= 'z') ||
						C == '!' || 
						C == '(' || 
						C == ')' ||
						C == '+' || 
						C == '-' || 
						C == '.' ||
						C == '=' || 
						C == '_' || 
						C == '~' || 
						C == '%';

		if (!bIsValid)
		{
			SanitizedString[Index] = '_';
		}
	}

	return SanitizedString;
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
			LoginSession = &NewLoginSession;
		}
	});
	UE_LOG(Log_BRVivox, Log, TEXT("Begin vivox login."));
	
	NewLoginSession.EventStateChanged.AddLambda([this](const LoginState& State)
	{
		if (State == LoginState::LoggedIn)
		{
			bIsLoggedIn = true; 
			UE_LOG(Log_BRVivox, Log, TEXT("Player successfully logged into Vivox."));
		}
		else
		{
			bIsLoggedIn = false; 
			UE_LOG(Log_BRVivox, Log, TEXT("Player disconnected from Vivox."));
		}
		UEventBus::Broadcast<const LoginState&>(this, GameInstance_Callback_OnVivoxLoginSessionStateChange, State);
	});
	
	NewLoginSession.BeginLogin("https://unity.vivox.com/appconfig/90721-td6_b-93439", 
		NewLoginSession.GetLoginToken(TokenKey, FTimespan::FromDays(1)), OnBeginLoginCompleted);

}

void UVivoxSubsystem::Logout()
{
	if (LoginSession)
	{
		LoginSession->Logout();
	}
}

void UVivoxSubsystem::JoinVocalRoom()
{
	if (!LoginSession)
	{
		UE_LOG(Log_BRVivox, Error, TEXT("User is not connected to Vivox, can't join vocal room !"));
		return;
	}
	
	TOptional<FString> SessionName = Chain::StartChain(UBROnlineSubsystem::Get(GetWorld()))
	.GetValue(&UBROnlineSubsystem::GetSessionName).Get(NullOpt);
	
	
	if (!SessionName)
	{
		UE_LOG(Log_BRVivox, Error, TEXT("Unable to retrieve session name, can't join vocal room !"));
		return;
	}
		
	ChannelId NewChannelId(TokenIssuer, FormatChannelSessionName(*SessionName), Domain, ChannelType::Positional);
	IChannelSession& NewChannelSession(LoginSession->GetChannelSession(NewChannelId));

	IChannelSession::FOnBeginConnectCompletedDelegate OnBeginConnectCompleted;

	OnBeginConnectCompleted.BindLambda([this, &NewChannelSession](VivoxCoreError Error)
	{
		if (VxErrorSuccess == Error)
		{
			ChannelSession = &NewChannelSession;
			Set3DPosition(FVector::ZeroVector, FVector::ForwardVector, FVector::UpVector);
		}
	});
	
	NewChannelSession.EventChannelStateChanged.AddLambda([this](const IChannelConnectionState& State)
	{
		FString ChannelName(State.ChannelSession().Channel().Name());
		if (ConnectionState::Connected == State.State())
		{
			bIsInVocalRoom = true;
			VoiceClient->AudioInputDevices().SetMuted(false);
			VoiceClient->AudioOutputDevices().SetMuted(false);
			LoginSession->SetTransmissionMode(TransmissionMode::All);
			
			OnLocalPlayerMove = UEventBus::AddLambda(this, Character_Callback_OnPlayerMove, [this](const FTransform& NewTransform)
			{
				Set3DPosition(NewTransform.GetLocation(), NewTransform.GetUnitAxis(EAxis::X), NewTransform.GetUnitAxis(EAxis::Z));
			});
			
			UE_LOG(Log_BRVivox, Log, TEXT("Channel %s fully connected audio state is connected = %d"), *ChannelName, (int)(ChannelSession->AudioState() == ConnectionState::Connected));
		}
		else if (ConnectionState::Disconnected == State.State())
		{
			UE_LOG(Log_BRVivox, Log, TEXT("Channel %s fully disconnected\n"), *ChannelName);
			bIsInVocalRoom = false;
			UEventBus::Remove(this, Character_Callback_OnPlayerMove, OnLocalPlayerMove);
		}
		UEventBus::Broadcast<const IChannelConnectionState&>(this, GameInstance_Callback_OnVivoxChannelSessionStateChange, State);
	});
	
	NewChannelSession.BeginConnect(true, false, true, NewChannelSession.GetConnectToken(
		TokenKey, FTimespan::FromDays(1)), OnBeginConnectCompleted);	
}

void UVivoxSubsystem::LeaveVocalRoom()
{
	if (bIsInVocalRoom && ChannelSession)
	{
		ChannelSession->Disconnect();
	}
}

void UVivoxSubsystem::Set3DPosition(const FVector& Position, const FVector& Forward, const FVector& Up)
{
	if (ChannelSession)
	{
		ChannelSession->Set3DPosition(Position, Position, Forward, Up);
	}
}
