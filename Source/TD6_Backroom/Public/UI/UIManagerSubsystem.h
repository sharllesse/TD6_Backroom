// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Chain.h"
#include "Linq.h"
#include "Blueprint/UserWidget.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "UIManagerSubsystem.generated.h"

/**
 * 
 */

UCLASS()
class TD6_BACKROOM_API UUIManager_Data : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UUserWidget>> WidgetList;
};


UCLASS()
class TD6_BACKROOM_API UUIManagerSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	TObjectPtr<UUIManager_Data> ManagerData;

	UPROPERTY()
	TArray<TObjectPtr<UUserWidget>> WidgetStack;
public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	template<typename T>
	requires std::is_base_of_v<UUserWidget, T>
	T* PushMenu(bool bHidePreviousMenu = true)
	{
		auto NewWidget = Linq::Start(ManagerData->WidgetList).First([](const TSubclassOf<UUserWidget>& Widget)
		{
			return Widget->IsChildOf(T::StaticClass());
		});
		
		if(!NewWidget)
		{
			return nullptr;
		}
		
		auto WidgetInstance = CreateWidget(GetLocalPlayer()->GetPlayerController(GetWorld()), *NewWidget);
		Chain::Execute(WidgetInstance,[](UUserWidget* Widget)
				{
					Widget->AddToViewport();
				});
		if (bHidePreviousMenu)
		{
			for (auto Element : WidgetStack)
			{
				Chain::Execute(Element.Get(), [](UUserWidget* Widget)
				{
					Widget->SetVisibility(ESlateVisibility::Collapsed);
				});
			}
		}
		
		WidgetStack.Add(WidgetInstance);
		return Cast<T>(WidgetInstance);
		
		return nullptr;
	}

	void PopMenu()
	{
		if (WidgetStack.IsEmpty())
		{
			return;
		}

		WidgetStack.Last()->RemoveFromParent();w
		WidgetStack.RemoveAt(WidgetStack.Num() - 1);

		if (WidgetStack.IsEmpty())
		{
			WidgetStack.Last()->SetVisibility(ESlateVisibility::Visible);
		}
		
	}
};
