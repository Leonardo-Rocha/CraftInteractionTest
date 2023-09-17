// Copyright Epic Games, Inc. All Rights Reserved.

#include "CraftInteractionTestGameMode.h"
#include "CraftInteractionTestCharacter.h"
#include "UObject/ConstructorHelpers.h"

ACraftInteractionTestGameMode::ACraftInteractionTestGameMode()
	: Super()
{
	// I don't know why unreal still ships code like this, if you rename or move the file it will crash the game.
	// DefaultPawnClass is already UPROPERTY exposed, so we only need to create a child of ACraftInteractionTestGameMode in BP.

	// set default pawn class to our Blueprinted character
	/*static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;*/
}