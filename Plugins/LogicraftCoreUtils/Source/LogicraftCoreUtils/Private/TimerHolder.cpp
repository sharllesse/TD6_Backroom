// Copyright (c) Logicraft Interactive. All Rights Reserved.

#include "TimerHolder.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

#if WITH_EDITOR
#include "Editor.h"
#endif

#define TIMER_HOLDER_CHECK() \
		checkf(TimerManager != nullptr, TEXT("The timer manager was not retrieved prior to using this function."))

#define TIMER_HOLDER_ENSURE() \
		ensureMsgf(RetrieveTimerManager(), TEXT("Unable to retrieve the timer manager because no valid context was found."))

FTimerHolder::~FTimerHolder()
{
	if (RetrieveTimerManager())
	{
		TimerManager->ClearTimer(TimerHandle);	
	}
}

void FTimerHolder::Pause()
{
	if (TIMER_HOLDER_ENSURE())
	{
		TimerManager->PauseTimer(TimerHandle);
	}
}

void FTimerHolder::Clear()
{
	if (TIMER_HOLDER_ENSURE())
	{
		TimerManager->ClearTimer(TimerHandle);
	}
}

bool FTimerHolder::IsPaused()
{
	if (TIMER_HOLDER_ENSURE())
	{
		return TimerManager->IsTimerPaused(TimerHandle);	
	}

	return true;
}

bool FTimerHolder::IsAlreadyRunning()
{
	if (TIMER_HOLDER_ENSURE())
	{
		return TimerManager->TimerExists(TimerHandle);	
	}

	return false;
}

float FTimerHolder::GetElapsedTime() const
{
	TIMER_HOLDER_CHECK();
	return TimerManager->GetTimerElapsed(TimerHandle);
}

float FTimerHolder::GetRate() const
{
	TIMER_HOLDER_CHECK();
	return TimerManager->GetTimerRate(TimerHandle);
}

float FTimerHolder::GetRemainingTime() const
{
	TIMER_HOLDER_CHECK();
	return TimerManager->GetTimerRemaining(TimerHandle);
}

bool FTimerHolder::	RetrieveTimerManager()
{
	if (TimerManager)
	{
		return true;
	}

	if (GEngine)
	{
		for (const FWorldContext& Context : GEngine->GetWorldContexts())
		{
			if (Context.WorldType == EWorldType::Game || Context.WorldType == EWorldType::PIE)
			{
				if (UWorld* World = Context.World())
				{
					TimerManager = &World->GetTimerManager();
					return true;
				}
			}
		}
	}

#if WITH_EDITOR
	if (!GEditor)
	{
		return false;
	}

	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
	if (!EditorWorld)
	{
		return false;
	}

	TimerManager = &EditorWorld->GetTimerManager();
	return true;
#else
	return false;
#endif
}
	
	

