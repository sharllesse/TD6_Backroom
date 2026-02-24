// Copyright (c) Logicraft Interactive. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/AIModule/Classes/Tasks/AITask.h"
#include "AITask_Base.generated.h"

/**
 * 
 */
UCLASS()
class TD6_BACKROOM_API UAITask_Base : public UAITask
{
	GENERATED_BODY()
	
	bool bHasFailed{ false };
	
public:
	/**
	 * @brief Create an AITask owned by an AIController and a TaskOwner.
	 * @param AIOwner     The AIController which own this AITask.
	 * @param InTaskOwner The BTTask that own this AITask and is connected with the callback.
	 * @return The AITask which must be contained somewhere to stop it when its owner (BTask) is aborted or finished.
	 *		   Be aware nullptr could be return.
	 */
	template<typename TAITask>
	static TAITask* Create(AAIController& AIOwner, IGameplayTaskOwnerInterface& InTaskOwner)
	{
		if (TAITask* NewTask{ NewAITask<TAITask>(AIOwner, InTaskOwner) })
		{
			return NewTask;
		}
		
		return nullptr;
	}
	
	/**
	 * @brief Checks if the task has failed.
	 * @return True if the task has failed, false otherwise.
	 */
	bool HasFailed() const;

	/**
	 * @brief Finishes the task successfully.
	 */
	void FinishTask();

	/**
	 * @brief Finishes the task with a failure state.
	 */
	void FinishFailedTask();
protected:
	/**
	 * @brief Virtual method for cleaning up resources. To be overridden by child classes.
	 */
	virtual void Clean() {}
};
