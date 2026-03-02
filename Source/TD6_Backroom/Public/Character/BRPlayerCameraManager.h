// ABRPlayerCameraManager.h
#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "BRPlayerCameraManager.generated.h"

UCLASS()
class TD6_BACKROOM_API ABRPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:
	ABRPlayerCameraManager();

	UPROPERTY(EditDefaultsOnly, Category = "Spectator Camera")
	float SpectatorThirdPersonDistance{300.f};

	UPROPERTY(EditDefaultsOnly, Category = "Spectator Camera")
	float SpectatorHeightOffset{50.f};

protected:
	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;
};