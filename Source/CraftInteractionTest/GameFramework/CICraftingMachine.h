// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CICraftingMachine.generated.h"

class USphereComponent;
class ACIItemInstance;
struct FRecipe;

UCLASS()
class CRAFTINTERACTIONTEST_API ACICraftingMachine : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACICraftingMachine();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collision, meta = (AllowPrivateAccess = "true"))
	USphereComponent* CollisionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Crafting, meta = (AllowPrivateAccess = "true"))
	TArray<FDataTableRowHandle> AvailableRecipes;

	UPROPERTY()
	TArray<ACIItemInstance*> OverlappingItems;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void CraftFirstPossibleRecipe();

public:
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
