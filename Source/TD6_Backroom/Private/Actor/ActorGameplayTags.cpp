// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/ActorGameplayTags.h"

UE_DEFINE_GAMEPLAY_TAG_COMMENT(Actor_ExitZone_Callback_OnPlayerEnter, "Actor.ExitZone.Callback.OnPlayerEnter",
								"A callback that is called when an player enter in the exit zone.\n"
								"[Args: AActor* Player]")


UE_DEFINE_GAMEPLAY_TAG_COMMENT(Actor_ExitZone_Callback_OnPlayerLeave, "Actor.ExitZone.Callback.OnPlayerLeave",
								"A callback that is called when an player leave in the exit zone.\n"
								"[Args: AActor* Player]")