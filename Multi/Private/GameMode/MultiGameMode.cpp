// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameMode/MultiGameMode.h"
#include "Character/MultiCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMultiGameMode::AMultiGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
