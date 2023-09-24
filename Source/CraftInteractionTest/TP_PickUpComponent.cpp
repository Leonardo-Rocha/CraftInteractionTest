// Copyright Epic Games, Inc. All Rights Reserved.

#include "TP_PickUpComponent.h"

#include <Net/UnrealNetwork.h>
#include <Kismet/KismetSystemLibrary.h>
#include <Net/Core/PushModel/PushModel.h>

UTP_PickUpComponent::UTP_PickUpComponent()
{
	// Setup the Sphere Collision
	SphereRadius = 32.f;

	SetIsReplicatedByDefault(true);
}

void UTP_PickUpComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACraftInteractionTestCharacter>(GetOwner());
}

void UTP_PickUpComponent::OnRep_CurrentAttachedPickUp()
{
	UKismetSystemLibrary::PrintString(GetOwner(), FString::Printf(TEXT("[%s] Received rep notify for obj %s."), ANSI_TO_TCHAR(__FUNCTION__), *CurrentAttachedPickUp->GetName()));

	CurrentAttachedPickUp->SetActorEnableCollision(false);

	// Attach the pickup to the First Person Character
	FAttachmentTransformRules attachmentRules(EAttachmentRule::SnapToTarget, true);
	CurrentAttachedPickUp->AttachToComponent(OwnerCharacter->GetMesh1P(), attachmentRules, FName(TEXT("PickupSocket")));

	// Set up action bindings
	if (auto playerController = Cast<APlayerController>(OwnerCharacter->GetController()))
	{
		if (playerController->IsLocalController())
		{
			// TODO: Add mapping context so users can drop the picked up item
			UKismetSystemLibrary::PrintString(GetOwner(), FString::Printf(TEXT("[%s] Setting up action bindings for local controller."), ANSI_TO_TCHAR(__FUNCTION__)));
		}

		//if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		//{
		//	// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
		//	Subsystem->AddMappingContext(FireMappingContext, 1);
		//}

		//if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		//{
		//	// Fire
		//	EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::Fire);
		//}
	}
}

void UTP_PickUpComponent::SetupPickup(AActor* newPickup)
{
	if (GetOwner()->HasAuthority())
	{
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, CurrentAttachedPickUp, this);

		CurrentAttachedPickUp = newPickup;

		OnRep_CurrentAttachedPickUp();
	}
}

void UTP_PickUpComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams sharedParams;
	sharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, CurrentAttachedPickUp, sharedParams);
}
