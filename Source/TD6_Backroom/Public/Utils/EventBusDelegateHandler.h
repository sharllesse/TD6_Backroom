// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EventBus.h"
#include "GameplayTagContainer.h"

/**
 * 
 */
class TD6_BACKROOM_API EventBusDelegateHandler
{
	TMap<FGameplayTag, TArray<FDelegateHandle>> DelegatesMap;
public:
	EventBusDelegateHandler();
	~EventBusDelegateHandler();

	template<typename Func>
	void AddDelegate(UObject* WorldContext, const FGameplayTag& Tag, Func&& Callback)
	{
		auto DelegateHandle = UEventBus::AddLambda(WorldContext, Tag, Forward<Func>(Callback));
		if (DelegatesMap.Find(Tag))
		{
			DelegatesMap[Tag].Add(MoveTemp(DelegateHandle));
		}
		else
		{
			DelegatesMap.Add(Tag, {MoveTemp(DelegateHandle)});
		}
	}

	void Clear(const UObject* WorldContext);
};
