// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CIItemInstance.h"
#include "CIPickup.generated.h"

struct FInteractionDefinition;

/**
 * 
 */
UCLASS()
class CRAFTINTERACTIONTEST_API ACIPickup : public ACIItemInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = Interaction)
	FInteractionDefinition PickupInteraction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interaction, ReplicatedUsing = OnRep_IsPickupAvailable)
	uint8 bIsPickUpAvailable : 1;

	ACIPickup();

	virtual void GetPossibleInteractions_Implementation(TMap<EInteractionType, FInteractionDefinition>& interactions) override;
	virtual void Interact_Implementation(EInteractionType interaction, ACraftInteractionTestCharacter* interactingCharacter) override;

	void SetIsPickupAvailable(bool newValue);

	UFUNCTION()
	void OnRep_IsPickupAvailable();

	void GetLifetimeReplicatedProps(TArray< class FLifetimeProperty >& OutLifetimeProps) const;
};
