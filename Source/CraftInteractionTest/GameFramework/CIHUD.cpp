// Fill out your copyright notice in the Description page of Project Settings.

#include "CIHUD.h"

#include <Blueprint/UserWidget.h>

void ACIHUD::BeginPlay()
{
	if (CrosshairWidgetClass)
	{
		CrosshairWidget = CreateWidget<UUserWidget>(GetOwningPlayerController(), CrosshairWidgetClass);

		if (CrosshairWidget)
		{
			CrosshairWidget->AddToViewport();
		}
	}
}
