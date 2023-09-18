// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Interfaces/CIInteractable.h"

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
	// Sets default values for this actor's properties
	ACIItemInstance();

	// Inherited via ICIInteractable
	void GetPossibleInteractions_Implementation(TMap<FName, FInteractionDefinition>& interactions) override;
	void Interact_Implementation(FName interaction, AActor* interactingActor) override;
	void OnFocusStart_Implementation() override;
	void OnFocusLost_Implementation() override;
};
