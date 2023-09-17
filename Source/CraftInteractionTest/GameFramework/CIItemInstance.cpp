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

void ACIItemInstance::OnFocusStart()
{
	if (Mesh)
	{
		Mesh->SetRenderCustomDepth(true);
	}
}

void ACIItemInstance::OnFocusLost()
{
	if (Mesh)
	{
		Mesh->SetRenderCustomDepth(false);
	}
}
