// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "CraftInteractionTestCharacter.h"

#include <EnhancedInputComponent.h>

#include "TP_PickUpComponent.generated.h"

class UInputMappingContext;
class UInputAction;
class ACIPickup;

// Declaration of the delegate that will be called when someone picks this up
// The character picking this up is the parameter sent with the notification
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPickUp, ACraftInteractionTestCharacter*, PickUpCharacter);

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CRAFTINTERACTIONTEST_API UTP_PickUpComponent : public USphereComponent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing= OnRep_CurrentAttachedPickUp, meta = (AllowPrivateAccess = "true"))
	ACIPickup* CurrentAttachedPickUp = nullptr;

	ACraftInteractionTestCharacter* OwnerCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	float MaxDropPickUpDistance = 500.f;

public:
	UTP_PickUpComponent();

	virtual void SetupPickup(ACIPickup* newPickup);

	virtual void DropPickUp();

protected:

	/** Called when the game starts */
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void ServerDropPickUp(FVector_NetQuantize dropLocation);
	void ServerDropPickUp_Implementation(FVector_NetQuantize dropLocation);

	UFUNCTION()
	void OnRep_CurrentAttachedPickUp();

	void GetLifetimeReplicatedProps(TArray< class FLifetimeProperty >& OutLifetimeProps) const;
};