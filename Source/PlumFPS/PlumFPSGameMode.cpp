// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlumFPSGameMode.h"
#include "PlumFPSHUD.h"
#include "PlumFPSCharacter.h"
#include "UObject/ConstructorHelpers.h"

APlumFPSGameMode::APlumFPSGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = APlumFPSHUD::StaticClass();
}
