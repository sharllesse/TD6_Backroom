// Copyright (c) Logicraft Interactive. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "BROnlineSettings.generated.h"

/**
 * 
 */
UCLASS(Config=Game, defaultconfig, DisplayName = "Online Settings")
class OVERWRITE_API UOWOnlineSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	/** If true, will attempt a connection to the selected service. */
	UPROPERTY(Config, EditAnywhere, Category = "Login")
	bool bAutoLogin{ true };
	
	static const ThisClass* Get() { return GetDefault<UOWOnlineSettings>(); }
};
