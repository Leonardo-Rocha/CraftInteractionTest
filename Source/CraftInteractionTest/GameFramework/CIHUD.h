// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CIHUD.generated.h"

class UUserWidget;

/**
 *
 */
UCLASS()
class CRAFTINTERACTIONTEST_API ACIHUD : public AHUD
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = UI)
	TSubclassOf<UUserWidget> CrosshairWidgetClass = nullptr;

	UUserWidget* CrosshairWidget = nullptr;

public:
	virtual void BeginPlay() override;
};
