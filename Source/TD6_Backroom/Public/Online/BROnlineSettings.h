// Copyright (c) Logicraft Interactive. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "BROnlineSettings.generated.h"

/**
 * Default parameters used by the BROnlineSubsystem.
 */
UCLASS(Config=Game, defaultconfig, DisplayName = "Online Settings")
class TD6_BACKROOM_API UBROnlineSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	/** If true, will attempt a connection to the selected service at the game start. */
	UPROPERTY(Config, EditAnywhere, Category = "Login")
	bool bAutoLogin{ true };

	/** If true, will attempt a query of the friends list of the selected service after the logging has been completed. */
	UPROPERTY(Config, EditAnywhere, Category = "Friends")
	bool bAutoQueryFriends{ true };

	/**
	 * If true, will attempt a query of the friends list of the selected service.
	 * This query is launched when change has occurred on the friends list (someone that has been blocked, unfriend, friend)
	 */
	UPROPERTY(Config, EditAnywhere, Category = "Friends")
	bool bAutoQueryFriendsOnFriendsListChange{ true };

	/** If true, will attempt to load the selected map after a session has been successfully created. */
	UPROPERTY(Config, EditAnywhere, Category = "Session|Create")
	bool bAutoLoadMapOnSessionCreation{ true };
	
	UPROPERTY(Config, EditAnywhere, Category = "Session|Create", meta = (AllowedClasses = "/Script/Engine.World", EditCondition = "bAutoLoadMapOnSessionCreation"))
	FSoftObjectPath LoadedMapOnSessionCreation;

	/**
	 * Parameters used to load the map in LoadedMapOnSessionCreation,
	 * listen is placed by default removing this options could cause a crash.
	 */
	UPROPERTY(Config, EditAnywhere, Category = "Session|Create", meta = (EditCondition = "bAutoLoadMapOnSessionCreation"))
	FString LoadedMapOptionsOnSessionCreation{ TEXT("listen") };

	/** If true, will attempt to load the selected map after a session has been successfully created. */
	UPROPERTY(Config, EditAnywhere, Category = "Session|Start")
	bool bAutoLoadMapOnSessionStart{ true };
	
	UPROPERTY(Config, EditAnywhere, Category = "Session|Start", meta = (AllowedClasses = "/Script/Engine.World", EditCondition = "bAutoLoadMapOnSessionStart"))
	FSoftObjectPath LoadedMapOnSessionStart;

	/**
	 * Parameters used to load the map in LoadedMapOnSessionCreation,
	 * listen is placed by default removing this options could cause a crash.
	 */
	UPROPERTY(Config, EditAnywhere, Category = "Session|Start", meta = (EditCondition = "bAutoLoadMapOnSessionStart"))
	FString LoadedMapOptionsOnSessionStart{ TEXT("listen") };

		
	UPROPERTY(Config, EditAnywhere, Category = "Session|Disconnection", meta = (AllowedClasses = "/Script/Engine.World"))
	FSoftObjectPath LoadedMapOnSessionDisconnection;
	
	static const ThisClass* Get() { return GetDefault<UBROnlineSettings>(); }
};
