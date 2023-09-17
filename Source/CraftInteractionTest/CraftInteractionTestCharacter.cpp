// Copyright Epic Games, Inc. All Rights Reserved.

#include "CraftInteractionTestCharacter.h"
#include "CraftInteractionTestProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CIItemInstance.h"
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
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	InteractionTraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
	InteractionTraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
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
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACraftInteractionTestCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACraftInteractionTestCharacter::Look);
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

		// Ignoring all attached actors - e.g. weapon
		TArray<AActor*> actorsToIgnore;
		GetAttachedActors(actorsToIgnore);

		FHitResult hit(ForceInit);
		UKismetSystemLibrary::LineTraceSingleForObjects(this, traceStart, traceEnd, InteractionTraceObjectTypes, true, actorsToIgnore, EDrawDebugTrace::ForOneFrame, hit, true);
		
		// TODO: use interface instead of casting to the instance directly.
		if (const auto hitActor = Cast<ACIItemInstance>(hit.GetActor()))
		{
			if (CurrentFocusObject && hitActor != CurrentFocusObject)
			{
				CurrentFocusObject->OnFocusLost();
			}

			CurrentFocusObject = hitActor;
			CurrentFocusObject->OnFocusStart();
		}
		else if (CurrentFocusObject)
		{
			CurrentFocusObject->OnFocusLost();
		}
	}
}

void ACraftInteractionTestCharacter::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool ACraftInteractionTestCharacter::GetHasRifle()
{
	return bHasRifle;
}