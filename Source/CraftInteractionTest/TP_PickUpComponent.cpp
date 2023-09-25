// Copyright Epic Games, Inc. All Rights Reserved.

#include "TP_PickUpComponent.h"
#include "GameFramework/CIPickup.h"

#include <Kismet/KismetSystemLibrary.h>
#include <Net/UnrealNetwork.h>
#include <Net/Core/PushModel/PushModel.h>
#include <EnhancedInputSubsystems.h>

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

void UTP_PickUpComponent::DropPickUp()
{
	if (!CurrentAttachedPickUp)
	{
		return;
	}

	// Ignoring all attached actors - e.g. weapon or picked up items
	TArray<AActor*> actorsToIgnore;
	GetOwner()->GetAttachedActors(actorsToIgnore);

	FVector cameraLocation;
	FRotator cameraRotation;
	OwnerCharacter->GetController()->GetPlayerViewPoint(cameraLocation, cameraRotation);

	const FVector traceStart = cameraLocation;
	// TRotator::Vector already returns a unit direction vector
	const FVector traceEnd = traceStart + (cameraRotation.Vector() * MaxDropPickUpDistance);
	TArray<TEnumAsByte<EObjectTypeQuery>> dropPickUpTraceObjectTypes = { UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic), UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic) };

	FHitResult hit(ForceInit);
	bool bTraceComplex = true;
	bool bIgnoreSelf = true;

	// We'll send this location so it feels less worse for the autonomous proxy who's dropping the 
	UKismetSystemLibrary::LineTraceSingleForObjects(GetOwner(), traceStart, traceEnd, dropPickUpTraceObjectTypes, bTraceComplex, actorsToIgnore, EDrawDebugTrace::ForDuration, hit, bIgnoreSelf);

	if (hit.bBlockingHit)
	{
		ServerDropPickUp(hit.Location);
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("[%s] Invalid drop location."), ANSI_TO_TCHAR(__FUNCTION__));
	}
}

void UTP_PickUpComponent::ServerDropPickUp_Implementation(FVector_NetQuantize dropLocation)
{
	// UKismetSystemLibrary::PrintString(GetOwner(), FString::Printf(TEXT("[%s] Received drop location: %s"), ANSI_TO_TCHAR(__FUNCTION__), *dropLocation.ToString()));

	FDetachmentTransformRules detachmentRules(EDetachmentRule::KeepWorld, true);
	CurrentAttachedPickUp->DetachFromActor(detachmentRules);

	FVector minBounds;
	FVector maxBounds;
	CurrentAttachedPickUp->GetMesh()->GetLocalBounds(minBounds, maxBounds);
	float halfHeight = (maxBounds.Z - minBounds.Z) * 0.5f;

	// Supposing all the items meshes always have origin in the center, we can adjust the Z-coordinate of the item's location by adding half of the mesh's height.
	dropLocation.Z += halfHeight;
	
	auto newTransform = FTransform(dropLocation);

	CurrentAttachedPickUp->SetActorTransform(newTransform, true);
	CurrentAttachedPickUp->SetActorEnableCollision(true);

	// UKismetSystemLibrary::PrintString(GetOwner(), FString::Printf(TEXT("[%s] Adjusted location: %s"), ANSI_TO_TCHAR(__FUNCTION__), *CurrentAttachedPickUp->GetActorLocation().ToString()));

	SetupPickup(nullptr);
}

void UTP_PickUpComponent::OnRep_CurrentAttachedPickUp(ACIPickup* oldAttachedPickUp)
{
	bool bPickupWasDropped = oldAttachedPickUp && !CurrentAttachedPickUp;

	if (bPickupWasDropped)
	{
		if (GetOwner()->HasAuthority())
		{
			oldAttachedPickUp->SetIsPickupAvailable(true);

			// It was already detached and adjusted in authority
			return;
		}

		// UKismetSystemLibrary::PrintString(GetOwner(), FString::Printf(TEXT("[%s] Detaching pickup. New Location: %s"), ANSI_TO_TCHAR(__FUNCTION__), *oldAttachedPickUp->GetActorLocation().ToString()));

		// For the clients we simply detach and rely on replication to update the location based on server changes
		FDetachmentTransformRules detachmentRules(EDetachmentRule::KeepWorld, true);
		oldAttachedPickUp->DetachFromActor(detachmentRules);
		oldAttachedPickUp->SetActorEnableCollision(true);
	}
	else
	{
		// UKismetSystemLibrary::PrintString(GetOwner(), FString::Printf(TEXT("[%s] Received rep notify for obj %s."), ANSI_TO_TCHAR(__FUNCTION__), *CurrentAttachedPickUp->GetName()));

		CurrentAttachedPickUp->SetActorEnableCollision(false);

		// Attach the pickup to the First Person Character
		FAttachmentTransformRules attachmentRules(EAttachmentRule::SnapToTarget, true);
		CurrentAttachedPickUp->AttachToComponent(OwnerCharacter->GetMesh1P(), attachmentRules, FName(TEXT("PickupSocket")));
	}
}

void UTP_PickUpComponent::SetupPickup(ACIPickup* newPickup)
{
	if (GetOwner()->HasAuthority())
	{
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, CurrentAttachedPickUp, this);

		ACIPickup* oldPickup = CurrentAttachedPickUp;
		CurrentAttachedPickUp = newPickup;

		OnRep_CurrentAttachedPickUp(oldPickup);
	}
}

void UTP_PickUpComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams sharedParams;
	sharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, CurrentAttachedPickUp, sharedParams);
}
