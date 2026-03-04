// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/BRGameGameMode.h"

#include "Chain.h"
#include "EventBus.h"
#include "Linq.h"
#include "Actor/ActorGameplayTags.h"
#include "Algo/AllOf.h"
#include "Algo/Count.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerState.h"
#include "GameMode/BRGameModeGameplayTags.h"
#include "GameState/BRGameGameState.h"
#include "Items/BRItemGameplayTag.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerState/BRPlayerStateGameTags.h"

void ABRGameGameMode::BeginPlay()
{
	Super::BeginPlay();

	VhsToCollect = FMath::RandRange(VhsToCollectRange.X, VhsToCollectRange.Y);

	Chain::Execute(GetGameState<ABRGameGameState>(), [this](ABRGameGameState* Game)
	{
		Game->VhsToCollect = VhsToCollect;
	});
	
	DelegateHandler.AddDelegate(this, Item_Callback_OnItemPickUp, [this](const FItemData& Item)
	{
		if (Item.Type.MatchesTagExact(Item_VHS))
			CheckIfHasAllVhs();		
	});

	DelegateHandler.AddDelegate(this, Actor_ExitZone_Callback_OnPlayerEnter, [this](AActor* Player)
	{
		++CurrentPlayerNumberInExitZone;
		CheckIfAllPlayerAreInExitZone();
	});
	
	DelegateHandler.AddDelegate(this, Actor_ExitZone_Callback_OnPlayerLeave, [this](AActor* Player)
	{
		--CurrentPlayerNumberInExitZone;
	});
	
	DelegateHandler.AddDelegate(this, PlayerState_Callback_Dies, [this]
	{
		CheckIfAllPlayerAreDead();
		CheckIfAllPlayerAreInExitZone();
	});
	
	UEventBus::Broadcast(this, GameMode_Callback_OnBeginPlayFinish);

}

void ABRGameGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	DelegateHandler.Clear(this);
}

void ABRGameGameMode::CheckIfHasAllVhs()
{
	if (bHasAllVhs)
	{
		return;
	}
	
	Chain::Execute(GetGameState<ABRGameGameState>(), [this](ABRGameGameState* Game)
	{
		if (auto Item = Game->GetItem(Item_VHS))
		{
			if (Item->Count >= VhsToCollect)
			{
				bHasAllVhs = true;
				Game->bAllObjectiveIsCompleted = true;
				Game->OnObjectivesCompleted_Rep();				
			}
		}
	});
}

void ABRGameGameMode::CheckIfAllPlayerAreInExitZone()
{
	if (bAllPlayerAreInExitZone || bAllPlayerAreDead)
	{
		return;
	}
	
	Chain::Execute(GetGameState<ABRGameGameState>(), [this](ABRGameGameState* Game)
	{
		if (CurrentPlayerNumberInExitZone >= Algo::CountIf(Game->PlayerArray, [](TObjectPtr<APlayerState> PlayerState)
		{
			return !PlayerState->IsSpectator();
		}))
		{
			bAllPlayerAreInExitZone = true;
			Game->MulticastNotifyAllPlayerAreInExitZone();
		}
	});
	
}

void ABRGameGameMode::CheckIfAllPlayerAreDead()
{
	if (bAllPlayerAreDead || bAllPlayerAreInExitZone)
	{
		return;
	}
	
	Chain::Execute(GetGameState<ABRGameGameState>(), [this](ABRGameGameState* Game)
	{
		if (Algo::AllOf(Game->PlayerArray, [](TObjectPtr<APlayerState> PlayerState)
		{
			return PlayerState->IsSpectator();
		}))
		{
			bAllPlayerAreDead = true;
			Game->MulticastNotifyAllPlayerAreDead();
		}
	});
}

AActor* ABRGameGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	AActor* RandomStart = GetRandomPlayerStart();
    
	if (RandomStart)
	{
		return RandomStart;
	}
	
	return Super::ChoosePlayerStart_Implementation(Player);
}

AActor* ABRGameGameMode::GetRandomPlayerStart()
{
	TArray<AActor*> FoundPlayerStarts;
    
	// Retrieve all PlayerStart actors currently existing in the level
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), FoundPlayerStarts);

	if (FoundPlayerStarts.Num() > 0)
	{
		// Select a random index within the bounds of the array
		int32 RandomIndex = FMath::RandRange(0, FoundPlayerStarts.Num() - 1);
		return FoundPlayerStarts[RandomIndex];
	}

	UE_LOG(LogTemp, Warning, TEXT("No PlayerStart actors found in the level."));
	return nullptr;
}

int ABRGameGameMode::GetVhsObjective() const
{
	return VhsToCollect;
}
