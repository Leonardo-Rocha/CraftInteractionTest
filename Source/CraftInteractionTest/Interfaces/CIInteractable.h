// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <UObject/Interface.h>

#include "CIInteractable.generated.h"

class ACraftInteractionTestCharacter;

USTRUCT(BlueprintType)
struct FInteractionDefinition
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bIsAvailable = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText InteractionPrompt;
};

UENUM(BlueprintType)
enum class EInteractionType : uint8
{
	None,
	IT_PrimaryInteraction,
	IT_SecondaryInteraction,
	// Add more interaction types as needed
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCIInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CRAFTINTERACTIONTEST_API ICIInteractable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void GetPossibleInteractions(TMap<EInteractionType, FInteractionDefinition>& interactions);
	virtual void GetPossibleInteractions_Implementation(TMap<EInteractionType, FInteractionDefinition>& interactions) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void Interact(EInteractionType interaction, ACraftInteractionTestCharacter* interactingCharacter);
	virtual void Interact_Implementation(EInteractionType interaction, ACraftInteractionTestCharacter* interactingCharacter) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void OnFocusStart();
	virtual void OnFocusStart_Implementation() = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void OnFocusLost();
	virtual void OnFocusLost_Implementation() = 0;

protected:
	TMap<EInteractionType, FInteractionDefinition> Interactions;
};
