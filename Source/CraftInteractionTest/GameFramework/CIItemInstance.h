// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CraftInteractionTest/Interfaces/CIInteractable.h"

#include <CoreMinimal.h>
#include <GameFramework/Actor.h>

#include "CIItemInstance.generated.h"

class UStaticMeshComponent;

UCLASS()
class CRAFTINTERACTIONTEST_API ACIItemInstance : public AActor, public ICIInteractable
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

public:	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
	FDataTableRowHandle ItemDefinition;

	// Sets default values for this actor's properties
	ACIItemInstance();

	// Inherited via ICIInteractable
	virtual void GetPossibleInteractions_Implementation(TMap<EInteractionType, FInteractionDefinition>& interactions) override;
	virtual void Interact_Implementation(EInteractionType interaction, ACraftInteractionTestCharacter* interactingCharacter) override;
	virtual void OnFocusStart_Implementation() override;
	virtual void OnFocusLost_Implementation() override;

	UStaticMeshComponent* GetMesh() const { return Mesh; }
};
