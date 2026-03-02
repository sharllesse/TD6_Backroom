// ABRPlayerCameraManager.cpp
#include "Character/BRPlayerCameraManager.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"

ABRPlayerCameraManager::ABRPlayerCameraManager()
{
}

void ABRPlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	Super::UpdateViewTarget(OutVT, DeltaTime);
}