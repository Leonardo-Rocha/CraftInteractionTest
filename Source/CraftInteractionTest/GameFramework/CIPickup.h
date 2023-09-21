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

	ACIPickup();

	virtual void GetPossibleInteractions_Implementation(TMap<EInteractionType, FInteractionDefinition>& interactions) override;
	virtual void Interact_Implementation(EInteractionType interaction, ACraftInteractionTestCharacter* interactingCharacter) override;

};
