// Copyright (c) Logicraft Interactive. All Rights Reserved.


#include "AI/Tasks/AITask_Base.h"

bool UAITask_Base::HasFailed() const
{
	return bHasFailed;
}

void UAITask_Base::FinishTask()
{
	UE_LOG(LogTemp, Warning, TEXT("Task Success"))
	
	ExternalCancel();
	Clean();
}

void UAITask_Base::FinishFailedTask()
{
	UE_LOG(LogTemp, Warning, TEXT("Task Failed"))
	
	bHasFailed = true;
	
	ExternalCancel();
	Clean();
}
