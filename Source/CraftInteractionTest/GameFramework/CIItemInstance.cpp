// Fill out your copyright notice in the Description page of Project Settings.


#include "CIItemInstance.h"

#include <Components/StaticMeshComponent.h>

// Sets default values
ACIItemInstance::ACIItemInstance()
{
 	// Turning this off to improve performance.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	SetRootComponent(Mesh);
}

void ACIItemInstance::GetPossibleInteractions_Implementation(TMap<EInteractionType, FInteractionDefinition>& interactions)
{
	interactions = Interactions;
}

void ACIItemInstance::Interact_Implementation(EInteractionType interaction, ACraftInteractionTestCharacter* interactingCharacter)
{
	// To be implemented in children
}

void ACIItemInstance::OnFocusStart_Implementation()
{
	if (Mesh)
	{
		Mesh->SetRenderCustomDepth(true);
	}
}

void ACIItemInstance::OnFocusLost_Implementation()
{
	if (Mesh)
	{
		Mesh->SetRenderCustomDepth(false);
	}
}
