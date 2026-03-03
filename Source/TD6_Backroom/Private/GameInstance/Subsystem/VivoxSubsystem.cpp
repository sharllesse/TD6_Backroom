// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstance/Subsystem/VivoxSubsystem.h"

#include "Chain.h"
#include "EventBus.h"
#include "IOnlineSubsystemEOS.h"
#include "Linq.h"
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

void UVivoxSubsystem::Join3DVocalRoom(const FString& SessionName)
{
	Channel3DProperties Properties(1800,800,1.f, EAudioFadeModel::InverseByDistance);
	
	ChannelId NewChannelId(TokenIssuer, FormatChannelSessionName(SessionName + TEXT("_Game")), Domain, ChannelType::Positional, Properties);
	IChannelSession& NewChannelSession(LoginSession->GetChannelSession(NewChannelId));

	IChannelSession::FOnBeginConnectCompletedDelegate OnBeginConnectCompleted;
	InGameChannelID = NewChannelId;
	
	
	OnBeginConnectCompleted.BindLambda([this, &NewChannelSession](VivoxCoreError Error)
	{
		if (VxErrorSuccess == Error)
		{
			InGameChannelSession = &NewChannelSession;
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
			LoginSession->SetTransmissionMode(TransmissionMode::Single, InGameChannelID);
						
			UE_LOG(Log_BRVivox, Log, TEXT("InGame Channel %s fully connected audio state is connected = %d"), *ChannelName, (int)(InGameChannelSession->AudioState() == ConnectionState::Connected));
		}
		else if (ConnectionState::Disconnected == State.State())
		{
			UE_LOG(Log_BRVivox, Log, TEXT("InGame Channel %s fully disconnected\n"), *ChannelName);
			bIsInVocalRoom = false;
		}
		UEventBus::Broadcast<const IChannelConnectionState&>(this, GameInstance_Callback_OnVivoxChannelSessionStateChange, State);
	});
	
	NewChannelSession.BeginConnect(true, false, false, NewChannelSession.GetConnectToken(
		TokenKey, FTimespan::FromDays(1)), OnBeginConnectCompleted);	
}

void UVivoxSubsystem::JoinLobbyVocalRoom(const FString& SessionName)
{	
	ChannelId NewChannelId(TokenIssuer, FormatChannelSessionName(SessionName+ TEXT("_Lobby")), Domain, ChannelType::NonPositional);
	IChannelSession& NewChannelSession(LoginSession->GetChannelSession(NewChannelId));
	LobbyChannelID = NewChannelId;

	IChannelSession::FOnBeginConnectCompletedDelegate OnBeginConnectCompleted;
	
	
	OnBeginConnectCompleted.BindLambda([this, &NewChannelSession](VivoxCoreError Error)
	{
		if (VxErrorSuccess == Error)
		{
			LobbyChannelSession = &NewChannelSession;
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
			LoginSession->SetTransmissionMode(TransmissionMode::Single, LobbyChannelID);
			
			UE_LOG(Log_BRVivox, Log, TEXT("Lobby Channel %s fully connected audio state is connected = %d"), *ChannelName, (int)(InGameChannelSession->AudioState() == ConnectionState::Connected));
		}
		else if (ConnectionState::Disconnected == State.State())
		{
			UE_LOG(Log_BRVivox, Log, TEXT("Lobby Channel %s fully disconnected\n"), *ChannelName);
			bIsInVocalRoom = false;
			UEventBus::Remove(this, Character_Callback_OnPlayerMove, OnLocalPlayerMove);
		}
		UEventBus::Broadcast<const IChannelConnectionState&>(this, GameInstance_Callback_OnVivoxChannelSessionStateChange, State);
	});
	
	NewChannelSession.BeginConnect(true, false, true, NewChannelSession.GetConnectToken(
		TokenKey, FTimespan::FromDays(1)), OnBeginConnectCompleted);	
}

void UVivoxSubsystem::Login()
{
	if (LoginSession)
		return;
	
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
	
	TOptional<FString> SessionUID = Chain::StartChain(UBROnlineSubsystem::Get(GetWorld()))
	.GetValue(&UBROnlineSubsystem::GetSessionUID).Get(NullOpt);
	
	if (!SessionName || !SessionUID)
	{
		UE_LOG(Log_BRVivox, Error, TEXT("Unable to retrieve session name or session unique id, can't join vocal room !"));
		return;
	}
	SessionName = SessionName->Append(*SessionUID);
	Join3DVocalRoom(*SessionName);
	JoinLobbyVocalRoom(*SessionName);
}

void UVivoxSubsystem::LeaveVocalRoom()
{
	if (bIsInVocalRoom && InGameChannelSession)
	{
		InGameChannelSession->Disconnect();
	}
}

void UVivoxSubsystem::SwitchTo3DRoom()
{
	if (!LoginSession || !LobbyChannelSession || !InGameChannelSession)
	{
		UE_LOG(Log_BRVivox, Error, TEXT("Error while switching to 3d channel, Login Session, LobbyChannelSession or InGameChannelSession was at nullptr"));
		return;
	}
	
	LoginSession->SetTransmissionMode(TransmissionMode::Single, InGameChannelID);
	if (LobbyChannelSession->AudioState() == ConnectionState::Connected)
	{
		
		LobbyChannelSession->BeginSetAudioConnected(false, false, 
			IChannelSession::FOnBeginSetAudioConnectedCompletedDelegate::CreateLambda([](VivoxCoreError Error)
		{
			if (VxErrorSuccess == Error)
			{
				UE_LOG(Log_BRVivox, Verbose, TEXT("Switch to 3D room channel was successful"));
			}
		}));
	}
	
	if (InGameChannelSession->AudioState() == ConnectionState::Connected)
	{
		InGameChannelSession->BeginSetAudioConnected(true, false, 
			IChannelSession::FOnBeginSetAudioConnectedCompletedDelegate::CreateLambda([](VivoxCoreError Error)
		{
			if (VxErrorSuccess == Error)
			{
				UE_LOG(Log_BRVivox, Verbose, TEXT("Switch to 2D room channel was successful"));
			}
		}));		
	}
}

void UVivoxSubsystem::SwitchTo2DRoom()
{
	if (!LoginSession || !LobbyChannelSession || !InGameChannelSession)
	{
		UE_LOG(Log_BRVivox, Error, TEXT("Error while switching to 2d channel, Login Session, LobbyChannelSession or InGameChannelSession was at nullptr"));
		return;
	}
	
	LoginSession->SetTransmissionMode(TransmissionMode::Single, LobbyChannelID);
	
	if (InGameChannelSession->AudioState() == ConnectionState::Connected)
	{
		InGameChannelSession->BeginSetAudioConnected(true, false, 
			IChannelSession::FOnBeginSetAudioConnectedCompletedDelegate::CreateLambda([](VivoxCoreError Error)
		{
			if (VxErrorSuccess == Error)
			{
				UE_LOG(Log_BRVivox, Verbose, TEXT("Switch to 2D room channel was successful"));
			}
		}));		
	}
	
	if (LobbyChannelSession->AudioState() == ConnectionState::Connected)
	{
		LobbyChannelSession->BeginSetAudioConnected(true, false, 
			IChannelSession::FOnBeginSetAudioConnectedCompletedDelegate::CreateLambda([](VivoxCoreError Error)
		{
			if (VxErrorSuccess == Error)
			{
				UE_LOG(Log_BRVivox, Verbose, TEXT("Switch to 2D room channel was successful"));
			}
		}));
	}
}

void UVivoxSubsystem::Set3DPosition(const FVector& Position, const FVector& Forward, const FVector& Up)
{
	if (InGameChannelSession)
	{
		InGameChannelSession->Set3DPosition(Position, Position, Forward, Up);
	}
}

TArray<FString> UVivoxSubsystem::GetAllMicrophones() const
{
	if (!VoiceClient)
	{
		return {};
	}
	
	return Linq::Start(VoiceClient->AudioInputDevices().AvailableDevices())
	.Select([](const TPair<FString,IAudioDevice*>& Element)
	{
		
		return Element.Value->Name();
	}).ToArray();
}

void UVivoxSubsystem::SetMicrophone(const FString& NewDevice) const
{
	if(VoiceClient)
	{
		IAudioDevice* Device{nullptr};
		for (auto PresenceKey : VoiceClient->AudioInputDevices().AvailableDevices())
		{
			if (NewDevice == PresenceKey.Value->Name())
			{
				Device = PresenceKey.Value;
				break;
			}
		}
		
		
		VoiceClient->AudioInputDevices().SetActiveDevice(*Device,
			IAudioDevices::FOnSetActiveDeviceCompletedDelegate::CreateLambda([this](VivoxCoreError Error, const FString & DeviceName)
			{
				if (Error == VxErrorSuccess)
				{
					UE_LOG(Log_BRVivox, Log, TEXT("Microphone change to %s"), *VoiceClient->AudioInputDevices().AvailableDevices()[DeviceName]->Name());
				}
			}));
	}
}

TArray<FString> UVivoxSubsystem::GetAllListeners() const
{
	if (!VoiceClient)
	{
		return {};
	}
	
	return Linq::Start(VoiceClient->AudioOutputDevices().AvailableDevices())
	.Select([](const TPair<FString,IAudioDevice*>& Element)
	{
		
		return Element.Value->Name();
	}).ToArray();
}

void UVivoxSubsystem::SetListener(const FString& NewDevice) const
{
	if(VoiceClient)
	{
		IAudioDevice* Device{nullptr};
		for (auto PresenceKey : VoiceClient->AudioOutputDevices().AvailableDevices())
		{
			if (NewDevice == PresenceKey.Value->Name())
			{
				Device = PresenceKey.Value;
				break;
			}
		}
		
		
		VoiceClient->AudioOutputDevices().SetActiveDevice(*Device,
			IAudioDevices::FOnSetActiveDeviceCompletedDelegate::CreateLambda([this](VivoxCoreError Error, const FString & DeviceName)
			{
				if (Error == VxErrorSuccess)
				{
					UE_LOG(Log_BRVivox, Log, TEXT("Microphone change to %s"), *VoiceClient->AudioOutputDevices().AvailableDevices()[DeviceName]->Name());
				}
			}));
	}
}
