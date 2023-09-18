// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <UObject/Interface.h>

#include "CIInteractable.generated.h"

class UInputAction;

USTRUCT(BlueprintType)
struct FInteractionDefinition
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bIsAvailable = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	TSoftObjectPtr<UInputAction> InputAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText InteractionPrompt;
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
	void GetPossibleInteractions(TMap<FName, FInteractionDefinition>& interactions);
	virtual void GetPossibleInteractions_Implementation(TMap<FName, FInteractionDefinition>& interactions) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void Interact(FName interaction, AActor* interactingActor);
	virtual void Interact_Implementation(FName interaction, AActor* interactingActor) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void OnFocusStart();
	virtual void OnFocusStart_Implementation() = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void OnFocusLost();
	virtual void OnFocusLost_Implementation() = 0;

protected:
	TMap<FName, FInteractionDefinition> Interactions;
};
