// Fill out your copyright notice in the Description page of Project Settings.


#include "CICraftingMachine.h"
#include "CraftInteractionTest/Types/CIItemTypes.h"
#include "CraftInteractionTest/GameFramework/CIItemInstance.h"

#include <Net/UnrealNetwork.h>
#include <Net/Core/PushModel/PushModel.h>
#include <Components/SphereComponent.h>

// Sets default values
ACICraftingMachine::ACICraftingMachine()
{
	// Turning this off to improve performance.
	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetupAttachment(RootComponent);

    bIsMachineOn = ToggleMachineInteraction.bIsAvailable;

    bReplicates = true;
}

// Called when the game starts or when spawned
void ACICraftingMachine::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereBeginOverlap);
		CollisionComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnSphereEndOverlap);
	}
}

void ACICraftingMachine::CraftFirstPossibleRecipe()
{
    if (!bIsMachineOn)
    {
        MulticastRecipeSpawnFailed();
    }

    TArray<ACIItemInstance*> ingredientsActors;
    FRecipe* possibleRecipe;

    // Iterate through the available recipes
    for (auto& recipe : AvailableRecipes)
    {
        bool bCanCraft = true;
        possibleRecipe = recipe.GetRow<FRecipe>(TEXT("Checking ingredients."));

        ingredientsActors.Reserve(possibleRecipe->Ingredients.Num());
        ingredientsActors.Empty();

        // Check if the recipe can be crafted based on the ingredients
        for (const auto& ingredient : possibleRecipe->Ingredients)
        {
            bool bFoundIngredient = false;

            // Iterate through the actors around the machine to find the required ingredients
            for (const auto& item : OverlappingItems)
            {
                // TODO: modify this algorithm to support quantities
                if (ingredient.Item == item->ItemDefinition)
                {
                    ingredientsActors.Add(item);
                    bFoundIngredient = true;
                    break;
                }
            }

            // If the required ingredient is not found, break the loop and move to the next recipe
            if (!bFoundIngredient)
            {
                bCanCraft = false;
                break;
            }
        }

        // If all required ingredients are found, craft the recipe
        if (bCanCraft)
        {
            // Destroy all the ingredients and remove them from the overlapping list
            for (auto& ingredient : ingredientsActors)
            {
                OverlappingItems.Remove(ingredient);
                ingredient->Destroy();
            }

            // Spawn crafted item
            SpawnItemFromRecipe(possibleRecipe);

            // Break the loop to stop crafting additional recipes
            break;
        }
    }
}

void ACICraftingMachine::SpawnItemFromRecipe(FRecipe* possibleRecipe)
{
    auto completedRecipeItemDefinition = possibleRecipe->ResultItem.GetRow<FItemDefinition>(TEXT("Completed Recipe"));
    if (const auto world = GetWorld())
    {
        FVector spawnLocation = GetActorLocation() + GetActorForwardVector() * 100.f;

        //Set Spawn Collision Handling Override
        FActorSpawnParameters spawnParams;
        spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        // Spawn the projectile at the muzzle
        world->SpawnActor<ACIItemInstance>(completedRecipeItemDefinition->ItemInstanceClass.LoadSynchronous(), spawnLocation, FRotator::ZeroRotator, spawnParams);

        MulticastRecipeCompleted();
    }
}

void ACICraftingMachine::OnRep_IsMachineOn()
{
    BP_OnMachineStateChanged();
}

void ACICraftingMachine::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// UE_LOG(LogTemp, Display, TEXT("Overlapping"));

    if (const auto itemInstance = Cast<ACIItemInstance>(OtherActor))
    {
	    OverlappingItems.AddUnique(itemInstance);

        CraftFirstPossibleRecipe();
    }
}

void ACICraftingMachine::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (const auto itemInstance = Cast<ACIItemInstance>(OtherActor))
    {
        OverlappingItems.Remove(itemInstance);
    }
}

void ACICraftingMachine::GetPossibleInteractions_Implementation(TMap<EInteractionType, FInteractionDefinition>& interactions)
{
    ToggleMachineInteraction.bIsAvailable = bIsMachineOn;

    interactions.Add(EInteractionType::IT_PrimaryInteraction, CompleteRandomRecipeInteraction);
    interactions.Add(EInteractionType::IT_SecondaryInteraction, ToggleMachineInteraction);
}

void ACICraftingMachine::Interact_Implementation(EInteractionType interaction, ACraftInteractionTestCharacter* interactingCharacter)
{
    switch (interaction)
    {
        case EInteractionType::IT_PrimaryInteraction:
        {
            if (!bIsMachineOn)
            {
                MulticastRecipeSpawnFailed();
                break;
            }

            // choose random recipe
            int32 randomIndex = FMath::RandRange(0, AvailableRecipes.Num() - 1);
            auto randomRecipe = AvailableRecipes[randomIndex].GetRow<FRecipe>(TEXT("Getting random recipe."));

            SpawnItemFromRecipe(randomRecipe);
            break;
        }
        case EInteractionType::IT_SecondaryInteraction:
        {
            SetIsMachineOn(!bIsMachineOn);
            break;
        }
        default:
        {
            UE_LOG(LogTemp, Error, TEXT("[%s] %s Interaction not defined for this interactable."), ANSI_TO_TCHAR(__FUNCTION__), *UEnum::GetValueAsString(interaction));
        }
    }
}

void ACICraftingMachine::OnFocusStart_Implementation()
{
}

void ACICraftingMachine::OnFocusLost_Implementation()
{
}

void ACICraftingMachine::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    FDoRepLifetimeParams sharedParams;
    sharedParams.bIsPushBased = true;

    DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bIsMachineOn, sharedParams);
}

void ACICraftingMachine::SetIsMachineOn(bool bNewValue)
{
    if (HasAuthority())
    {
        MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bIsMachineOn, this);

        bIsMachineOn = bNewValue;
    }
}

