// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/FEventBusDelegateHandler.h"


FEventBusDelegateHandler::FEventBusDelegateHandler()
{
}

FEventBusDelegateHandler::~FEventBusDelegateHandler()
{
}

void FEventBusDelegateHandler::Clear(const UObject* WorldContext)
{
	for (const auto& [Tag, Delegates] : DelegatesMap)
	{
		for (auto Delegate : Delegates)
		{
			UEventBus::Remove(WorldContext, Tag, Delegate);
		}
	}

	DelegatesMap.Empty();
}
