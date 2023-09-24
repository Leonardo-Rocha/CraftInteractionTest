// Copyright Epic Games, Inc. All Rights Reserved.

#include "CraftInteractionTestCharacter.h"
#include "CraftInteractionTestProjectile.h"
#include "Interfaces/CIInteractable.h"
#include "TP_PickUpComponent.h"

#include <Animation/AnimInstance.h>
#include <Camera/CameraComponent.h>
#include <Components/CapsuleComponent.h>
#include <EnhancedInputComponent.h>
#include <EnhancedInputSubsystems.h>
#include <Kismet/KismetSystemLibrary.h>


//////////////////////////////////////////////////////////////////////////
// ACraftInteractionTestCharacter

ACraftInteractionTestCharacter::ACraftInteractionTestCharacter()
{
	// Character doesnt have a rifle at start
	bHasRifle = false;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	// We'll enable visibility of the mesh so we don't have to do a 3P setup with animations and everything.
	// Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	PickUpComponent = CreateDefaultSubobject<UTP_PickUpComponent>(TEXT("PickUpComponent"));

	InteractionTraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
	InteractionTraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));

	bReplicates = true;
}

void ACraftInteractionTestCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Only standalone, clients and listen servers need to be scanning for interaction
	// Upon selecting an object to interact, clients send an RPC to start interaction
	if (Controller && Controller->IsLocalController())
	{
		GetWorld()->GetTimerManager().SetTimer(InteractionTimerHandle, this, &ThisClass::ScanInteractables, InteractionScanInterval, true);
	}
}

//////////////////////////////////////////////////////////////////////////// Input

void ACraftInteractionTestCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);

		//Interacting
		EnhancedInputComponent->BindAction(PrimaryInteraction, ETriggerEvent::Triggered, this, &ThisClass::ExecutePrimaryInteraction);

		// Drop PickUp
		EnhancedInputComponent->BindAction(DropPickUpAction, ETriggerEvent::Triggered, PickUpComponent, &UTP_PickUpComponent::DropPickUp);
	}
}


void ACraftInteractionTestCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void ACraftInteractionTestCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ACraftInteractionTestCharacter::ScanInteractables()
{
	if (Controller)
	{
		FVector cameraLocation;
		FRotator cameraRotation;
		Controller->GetPlayerViewPoint(cameraLocation, cameraRotation);

		const FVector traceStart = cameraLocation;
		// TRotator::Vector already returns a unit direction vector
		const FVector traceEnd = traceStart + (cameraRotation.Vector() * MaxInteractionDistance);

		// Ignoring all attached actors - e.g. weapon or picked up items
		TArray<AActor*> actorsToIgnore;
		GetAttachedActors(actorsToIgnore);

		FHitResult hit(ForceInit);
		bool bTraceComplex = true;
		bool bIgnoreSelf = true;
		UKismetSystemLibrary::LineTraceSingleForObjects(this, traceStart, traceEnd, InteractionTraceObjectTypes, bTraceComplex, actorsToIgnore, EDrawDebugTrace::ForOneFrame, hit, bIgnoreSelf);
		
		auto hitActor = hit.GetActor();
		if (hitActor && hitActor->GetClass()->ImplementsInterface(UCIInteractable::StaticClass()))
		{
			if (CurrentFocusObject && hitActor != CurrentFocusObject)
			{
				ICIInteractable::Execute_OnFocusLost(CurrentFocusObject);
			}

			CurrentFocusObject = hitActor;
			ICIInteractable::Execute_OnFocusStart(CurrentFocusObject);

			InteractionFocusChangedDelegate.Broadcast(CurrentFocusObject);
		}
		else if (CurrentFocusObject)
		{
			ICIInteractable::Execute_OnFocusLost(CurrentFocusObject);
			CurrentFocusObject = nullptr;

			InteractionFocusChangedDelegate.Broadcast(CurrentFocusObject);
		}
	}
}

void ACraftInteractionTestCharacter::ExecutePrimaryInteraction()
{
	// TODO: RPC to interact on server.
	// We don't check if it implements interface again because we only assign when it implements
	if (CurrentFocusObject)
	{
		TMap<EInteractionType, FInteractionDefinition> interactions;

		ICIInteractable::Execute_GetPossibleInteractions(CurrentFocusObject, interactions);

		if (interactions.Contains(EInteractionType::IT_PrimaryInteraction))
		{
			ServerExecuteInteraction(CurrentFocusObject, EInteractionType::IT_PrimaryInteraction);
		}
	}
}

void ACraftInteractionTestCharacter::ExecuteSecondaryInteraction()
{
}

void ACraftInteractionTestCharacter::ServerExecuteInteraction_Implementation(AActor* interactionObject, EInteractionType interactionType)
{
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Executing %s interaction in %s."), *UEnum::GetValueAsString(interactionType), *interactionObject->GetName()));
	ICIInteractable::Execute_Interact(interactionObject, interactionType, this);
}

bool ACraftInteractionTestCharacter::ServerExecuteInteraction_Validate(AActor* interactionObject, EInteractionType interactionType)
{
	// Here we validate if the client is actually near the object he's trying to interact with
	float distanceToInteractionObject = GetDistanceTo(interactionObject);
	return distanceToInteractionObject <= MaxInteractionDistance;
}

void ACraftInteractionTestCharacter::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool ACraftInteractionTestCharacter::GetHasRifle()
{
	return bHasRifle;
}