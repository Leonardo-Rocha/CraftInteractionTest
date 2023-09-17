// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <GameFramework/Actor.h>

#include "CIItemInstance.generated.h"

class UStaticMeshComponent;

UCLASS()
class CRAFTINTERACTIONTEST_API ACIItemInstance : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	UStaticMeshComponent* Mesh;
	
public:	
	// Sets default values for this actor's properties
	ACIItemInstance();

	virtual void OnFocusStart();
	virtual void OnFocusLost();
};
