// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BRCharacterGameplayTags.h"


UE_DEFINE_GAMEPLAY_TAG_COMMENT(Character_Callback_OnPlayerMove, "Character.Callback.OnPlayerMove",
								"A callback that is called when an character move from a controller.\n"
								"[Args: const FTransform& NewTransform]")


UE_DEFINE_GAMEPLAY_TAG_COMMENT(Character_Callback_OnPlayerSwitchSpectate, "Character.Callback.OnPlayerSwitchSpectate",
								"A callback that is called when an spectator switch his spectate target.\n"
								"[Args: const APlayerState* NewPlayer]")

