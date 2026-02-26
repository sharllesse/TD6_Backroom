// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/BRItemGameplayTag.h"

UE_DEFINE_GAMEPLAY_TAG(Item_VHS, "Item.VHS")

UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Callback_OnItemPickUp, "Item.Callback.OnItemPickUp",
	"A callback called when a new item is added to the shared inventory.\n"
	"[Args : const FItemData&]")
