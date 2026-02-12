// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "BRAsyncTaskDownloadImage.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnImageRetrievedDelegate, UTexture2DDynamic*, FUniqueNetIdWeakPtr)

/**
 * A basic copy of UAsyncTaskDownloadImage with one modification.
 * It takes a FUniqueNetIdWeakPtr.
 */
UCLASS()
class TD6_BACKROOM_API UBRAsyncTaskDownloadImage : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	FOnImageRetrievedDelegate OnSuccess;
	FOnImageRetrievedDelegate OnFail;

private:
	FUniqueNetIdWeakPtr UserId;
	
public:
	UBRAsyncTaskDownloadImage(const FObjectInitializer& ObjectInitializer);
	
	static UBRAsyncTaskDownloadImage* DownloadImage(FString URL, FUniqueNetIdWeakPtr UserId);

	void Start(FString URL);

private:
	/** Handles image requests coming from the web */
	void HandleImageRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
};
