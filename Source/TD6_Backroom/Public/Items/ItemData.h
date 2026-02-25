// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "ItemData.generated.h"

/**
 * 
 */



USTRUCT(Blueprintable, BlueprintType)
struct FItemData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	FGameplayTag Type;
	
	UPROPERTY(EditAnywhere)
	FString Name;

	UPROPERTY(EditAnywhere)
	FString Description;

	UPROPERTY()
	int Count{0};

	UPROPERTY(EditAnywhere)
	bool bIsShared{true};

	bool operator==(const FItemData& Other) const
	{
		return Other.Type.MatchesTagExact(Type);
	}
};

UCLASS()
class UItemDataTable : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, FItemData> ItemsData;

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	inline static TStrongObjectPtr<UItemDataTable> Self;

public:
	static void Init(UWorld* WorldContext);
	static void Destroy();

	static TOptional<FItemData> GetItemData(const FGameplayTag& Type);
	static const TMap<FGameplayTag, FItemData>& GetDataTable();
};