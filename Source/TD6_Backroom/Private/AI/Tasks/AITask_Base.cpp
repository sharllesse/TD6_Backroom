// Copyright (c) Logicraft Interactive. All Rights Reserved.


#include "AI/Tasks/AITask_Base.h"

bool UAITask_Base::HasFailed() const
{
	return bHasFailed;
}

void UAITask_Base::FinishTask()
{
	ExternalCancel();
	Clean();
}

void UAITask_Base::FinishFailedTask()
{
	bHasFailed = true;
	
	ExternalCancel();
	Clean();
}
