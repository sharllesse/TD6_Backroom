// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGameUI.h"

#include "Chain.h"
#include "EventBus.h"
#include "GameState/BRGameGameState.h"
#include "Items/BRItemGameplayTag.h"
#include "Items/ItemData.h"

void UInGameUI::NativeConstruct()
{
	Super::NativeConstruct();

	Chain::Execute(GetWorld()->GetGameState<ABRGameGameState>(), [this](const ABRGameGameState* GameState)
	{
		MaxVHS = GameState->VhsToCollect;
		auto Text = FText::FromString(FString::Printf(TEXT("%d/%d VHS"), 0, MaxVHS));
		VHSText->SetText(Text);
	});

	UpdateVHSTextHandle = UEventBus::AddLambda(this, Item_Callback_OnItemPickUp, [this](const FItemData& Item)
	{
		if (Item.Type == Item_VHS)
		{
			int CurrentVHs{0};
			Chain::Execute(GetWorld()->GetGameState<ABRGameGameState>(), [&](const ABRGameGameState* GameState)
			{
				CurrentVHs = FMath::Min(GameState->GetItem(Item.Type)->Count, MaxVHS);
			});
			
			auto Text = FText::FromString(FString::Printf(TEXT("%d/%d VHS"), CurrentVHs, MaxVHS));
			VHSText->SetText(Text);
		}
	});
}

void UInGameUI::NativeDestruct()
{
	Super::NativeDestruct();
	UEventBus::Remove(this, Item_Callback_OnItemPickUp, UpdateVHSTextHandle);
}

void UInGameUI::SetCanInteract(bool bCanInteract, const FText& InteractionText) const
{
	InteractText->SetVisibility(bCanInteract ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	InteractText->SetText(FText::FromString(FString::Printf(TEXT("Press E to %s"), *InteractionText.ToString())));
}
