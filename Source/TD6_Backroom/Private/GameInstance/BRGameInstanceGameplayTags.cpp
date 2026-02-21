// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstance/BRGameInstanceGameplayTags.h"


UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameInstance_Callback_OnVivoxLoginSessionStateChange, "GameInstance.Callback.OnVivoxLoginSessionStateChange",
								"A callback the login session state change.\n"
								"[Args: const LoginState& NewLoginState]")


UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameInstance_Callback_OnVivoxChannelSessionStateChange, "GameInstance.Callback.OnVivoxChannelSessionStateChange",
								"A callback the channel session state change.\n"
								"[Args: const IChannelConnectionState& NewState]")
