// Fill out your copyright notice in the Description page of Project Settings.


#include "CIPickup.h"
#include "CraftInteractionTest/CraftInteractionTestCharacter.h"
#include "CraftInteractionTest/TP_PickUpComponent.h"

#include <Net/UnrealNetwork.h>
#include <Net/Core/PushModel/PushModel.h>

ACIPickup::ACIPickup()
{
	PickupInteraction.bIsAvailable = true;
	// This text can be localized in the blueprint
	PickupInteraction.InteractionPrompt = FText::FromString(TEXT("Pickup"));

	Interactions.Add(EInteractionType::IT_PrimaryInteraction, PickupInteraction);

	bReplicates = true;
	SetReplicateMovement(true);

	bIsPickUpAvailable = PickupInteraction.bIsAvailable;
}

void ACIPickup::GetPossibleInteractions_Implementation(TMap<EInteractionType, FInteractionDefinition>& interactions)
{
	PickupInteraction.bIsAvailable = bIsPickUpAvailable;

	interactions.Add(EInteractionType::IT_PrimaryInteraction, PickupInteraction);
}

void ACIPickup::Interact_Implementation(EInteractionType interaction, ACraftInteractionTestCharacter* interactingCharacter)
{
	switch (interaction)
	{
		case EInteractionType::IT_PrimaryInteraction:
		{
			if (bIsPickUpAvailable)
			{
				UE_LOG(LogTemp, Display, TEXT("[%s] %s interaction available. Attempting to pick up."), ANSI_TO_TCHAR(__FUNCTION__), *UEnum::GetValueAsString(interaction));

				interactingCharacter->GetPickUpComponent()->SetupPickup(this);

				SetIsPickupAvailable(false);
			}
			else
			{
				UE_LOG(LogTemp, Display, TEXT("[%s] %s Interaction not available."), ANSI_TO_TCHAR(__FUNCTION__), *UEnum::GetValueAsString(interaction));
			}

			break;
		}
		default:
		{
			UE_LOG(LogTemp, Error, TEXT("[%s] %s Interaction not defined for this interactable."), ANSI_TO_TCHAR(__FUNCTION__), *UEnum::GetValueAsString(interaction));
		}
	}
}

void ACIPickup::SetIsPickupAvailable(bool newValue)
{
	if (HasAuthority())
	{
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bIsPickUpAvailable, this);

		bIsPickUpAvailable = newValue;

		OnRep_IsPickupAvailable();
	}
}

void ACIPickup::OnRep_IsPickupAvailable()
{
	if (bIsPickUpAvailable)
	{
		SetActorEnableCollision(true);
	}
}

void ACIPickup::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams sharedParams;
	sharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bIsPickUpAvailable, sharedParams);
}
