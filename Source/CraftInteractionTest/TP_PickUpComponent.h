// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "CraftInteractionTestCharacter.h"
#include "TP_PickUpComponent.generated.h"

class UInputMappingContext;
class UInputAction;
class ACIPickup;
struct FInputBindingHandle;

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

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* PickUpMappingContext;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DropPickUpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	float MaxDropPickUpDistance = 500.f;

public:
	UTP_PickUpComponent();

	virtual void SetupPickup(ACIPickup* newPickup);

protected:

	/** Called when the game starts */
	virtual void BeginPlay() override;

	virtual void DropPickUp();

	UFUNCTION(Server, Reliable)
	void ServerDropPickUp(FVector_NetQuantize dropLocation);
	void ServerDropPickUp_Implementation(FVector_NetQuantize dropLocation);

	UFUNCTION()
	void OnRep_CurrentAttachedPickUp(ACIPickup* oldAttachedPickUp);

	void GetLifetimeReplicatedProps(TArray< class FLifetimeProperty >& OutLifetimeProps) const;

private:
	FInputBindingHandle DropPickUpActionBindingHandle;
};