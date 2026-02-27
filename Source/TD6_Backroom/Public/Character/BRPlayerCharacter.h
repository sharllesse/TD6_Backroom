// Copyright (c) Logicraft Interactive. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "ActorComponent/InteractionComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameInstance/Subsystem/VivoxSubsystem.h"
#include "BRPlayerCharacter.generated.h"

class UInteractionComponent;

UCLASS()
class UBRPlayerData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float CrouchSpeed{150.f};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed{300.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed{600.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float AccelerationModifier{4.f};

	UPROPERTY(BlueprintReadWrite, Category = "Movement")
	bool bIsSprinting{false};
};

UCLASS()
class TD6_BACKROOM_API ABRPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	friend class ABRPlayerController;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<UInteractionComponent> InteractionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	TObjectPtr<UBRPlayerData> PlayerData;
public:
	// Sets default values for this character's properties
	ABRPlayerCharacter();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaSeconds) override;

	template<typename Func>
	void SetNotifyInteractCallback(Func&& Callback)
	{
		InteractionComponent->NotifyCanInteractEvent.BindLambda(Forward<Func>(Callback));
	}
protected:
	void OnMove(const FInputActionValue& InputActionValue);

	void OnJump(const FInputActionValue& InputActionValue);

	void OnSprint(const FInputActionValue& InputActionValue);
	
	UFUNCTION(Server, Reliable)
	void Server_OnSprint(bool bWantSprint);
	
	void UpdateSprintState(bool bShouldSprint);
	
	void OnTryInteract() const;

	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;
};

