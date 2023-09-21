// Fill out your copyright notice in the Description page of Project Settings.


#include "CIPickup.h"
#include "CraftInteractionTest/CraftInteractionTestCharacter.h"

ACIPickup::ACIPickup()
{
	PickupInteraction.bIsAvailable = true;
	// This text can be localized in the blueprint
	PickupInteraction.InteractionPrompt = FText::FromString(TEXT("Pickup"));

	Interactions.Add(EInteractionType::IT_PrimaryInteraction, PickupInteraction);
}

void ACIPickup::GetPossibleInteractions_Implementation(TMap<EInteractionType, FInteractionDefinition>& interactions)
{
	interactions = Interactions;
}

void ACIPickup::Interact_Implementation(EInteractionType interaction, ACraftInteractionTestCharacter* interactingCharacter)
{
	if (const auto foundInteraction = Interactions.Find(interaction))
	{
		if (foundInteraction->bIsAvailable)
		{
			// Attach to characters hands
			UE_LOG(LogTemp, Display, TEXT("[%s] %s interaction available. Attempting to pick up."), ANSI_TO_TCHAR(__FUNCTION__), *UEnum::GetValueAsString(interaction));

			SetActorEnableCollision(false);

			// TODO: Add mapping context so users can drop the picked up item

			// Attach the pickup to the First Person Character
			FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
			AttachToComponent(interactingCharacter->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("[%s] %s Interaction not available."), ANSI_TO_TCHAR(__FUNCTION__), *UEnum::GetValueAsString(interaction));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] %s Interaction not defined for this interactable."), ANSI_TO_TCHAR(__FUNCTION__), *UEnum::GetValueAsString(interaction));
	}
}
