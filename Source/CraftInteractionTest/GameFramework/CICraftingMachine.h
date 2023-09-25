// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CraftInteractionTest/Interfaces/CIInteractable.h"

#include "CICraftingMachine.generated.h"

class USphereComponent;
class ACIItemInstance;
struct FRecipe;

UCLASS()
class CRAFTINTERACTIONTEST_API ACICraftingMachine : public AActor, public ICIInteractable
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collision, meta = (AllowPrivateAccess = "true"))
	USphereComponent* CollisionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Crafting, meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	TArray<FDataTableRowHandle> AvailableRecipes;

	UPROPERTY()
	TArray<ACIItemInstance*> OverlappingItems;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crafting, ReplicatedUsing = OnRep_IsMachineOn, meta = (AllowPrivateAccess = "true"))
	uint8 bIsMachineOn : 1;
	
public:	
	UPROPERTY(EditDefaultsOnly, Category = Interaction)
	FInteractionDefinition ToggleMachineInteraction;

	UPROPERTY(EditDefaultsOnly, Category = Interaction)
	FInteractionDefinition CompleteRandomRecipeInteraction;

	// Sets default values for this actor's properties
	ACICraftingMachine();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void CraftFirstPossibleRecipe();

	void SpawnItemFromRecipe(FRecipe* possibleRecipe);

	UFUNCTION()
	void OnRep_IsMachineOn();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="On Machine State Changed"))
	void BP_OnMachineStateChanged();

public:
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// Inherited via ICIInteractable
	void GetPossibleInteractions_Implementation(TMap<EInteractionType, FInteractionDefinition>& interactions) override;
	void Interact_Implementation(EInteractionType interaction, ACraftInteractionTestCharacter* interactingCharacter) override;
	void OnFocusStart_Implementation() override;
	void OnFocusLost_Implementation() override;

	void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const;

	void SetIsMachineOn(bool bNewValue);
};
