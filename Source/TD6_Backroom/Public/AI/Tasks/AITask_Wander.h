// Copyright (c) Logicraft Interactive. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AITask_Base.h"
#include "AI/Tasks/OWAiType.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "AITask_Wander.generated.h"

/**
 * 
 */
UCLASS()
class TD6_BACKROOM_API UAITask_Wander : public UAITask_Base
{
	GENERATED_BODY()

public:
	FEQSParametrizedQueryExecutionRequest* EQSRequest;
	FMoveToParametrizedExecutionRequest* MoveToRequest;

private:
	FQueryFinishedSignature QueryFinishedSignature;
	int32 CachedQueryID{ 0 };
	FAIRequestID CachedMoveID{ FAIRequestID::InvalidRequest };

public:
	virtual void Activate() override;

protected:
	virtual void Clean() override;
	
private:
	void OnQueryRequestFinished(TSharedPtr<FEnvQueryResult> QueryResult);

	void OnMoveRequestFinished(FAIRequestID RequestID, const FPathFollowingResult& Result);
};
