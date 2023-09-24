// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "CraftInteractionTestCharacter.h"
#include "TP_PickUpComponent.generated.h"

// Declaration of the delegate that will be called when someone picks this up
// The character picking this up is the parameter sent with the notification
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPickUp, ACraftInteractionTestCharacter*, PickUpCharacter);

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CRAFTINTERACTIONTEST_API UTP_PickUpComponent : public USphereComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing= OnRep_CurrentAttachedPickUp)
	AActor* CurrentAttachedPickUp = nullptr;

	ACraftInteractionTestCharacter* OwnerCharacter;

	UTP_PickUpComponent();

	virtual void SetupPickup(AActor* newPickup);
protected:

	/** Called when the game starts */
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_CurrentAttachedPickUp();

	void GetLifetimeReplicatedProps(TArray< class FLifetimeProperty >& OutLifetimeProps) const;
};