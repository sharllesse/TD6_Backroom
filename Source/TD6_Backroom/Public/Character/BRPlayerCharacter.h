// Copyright (c) Logicraft Interactive. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "ActorComponent/InteractionComponent.h"
#include "ActorComponent/StaminaComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "BRPlayerCharacter.generated.h"

class UMediaSoundComponent;
class UScreamerWidget;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	TObjectPtr<USoundCue> NoStaminaSound;
};

UCLASS()
class TD6_BACKROOM_API ABRPlayerCharacter : public ACharacter, public IGenericTeamAgentInterface
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

	FGenericTeamId GenericTeamId;

	UPROPERTY(Transient)
	TObjectPtr<UScreamerWidget> ScreamerWidget;

	UPROPERTY()
	TObjectPtr<UMediaSoundComponent> ScreamerSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	TObjectPtr<UStaminaComponent> StaminaComponent;
public:
	// Sets default values for this character's properties
	ABRPlayerCharacter();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaSeconds) override;

	static inline FName Dead{ "Dead" };
protected:
	template<typename Func>
		void SetNotifyInteractCallback(Func&& Callback)
	{
		InteractionComponent->NotifyCanInteractEvent.BindLambda(Forward<Func>(Callback));
	}

	virtual void SetGenericTeamId(const FGenericTeamId& TeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
protected:
	void OnMove(const FInputActionValue& InputActionValue);

	void OnJump(const FInputActionValue& InputActionValue);

	void OnSprint(const FInputActionValue& InputActionValue);
	
	UFUNCTION(Server, Reliable)
	void Server_OnSprint(bool bWantSprint);
	
	void UpdateSprintState(bool bShouldSprint);
	
	void OnTryInteract() const;

	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;

public:
	UFUNCTION(NetMulticast, Reliable)
	void EnableRagdoll();

	void TriggerScreamer();
};
