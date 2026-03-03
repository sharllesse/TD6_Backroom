// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/BRGameModeGameplayTags.h"


UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameMode_Callback_OnAllPlayerInExitZone, "GameMode.Callback.OnAllPlayerInExitZone",
								"A callback for when all the player are in the exit zone, this trigger only once.\n"
								"[Args: ]")

UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameMode_Callback_OnAllPlayerAreDead, "GameMode.Callback.OnAllPlayerAreDead",
								"A callback for when all the player are dead, this trigger only once.\n"
								"[Args: ]")


UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameMode_Callback_OnBeginPlayFinish, "GameMode.Callback.OnBeginPlayFinish",
								"A callback for when the GameMode finish his BeginPlay.\n"
								"[Args: ]")