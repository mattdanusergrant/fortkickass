// Copyright Epic Games, Inc. All Rights Reserved.

#include "FortKickassGameMode.h"
#include "FortKickassCharacter.h"
#include "FortPlayerState.h"
#include "UObject/ConstructorHelpers.h"

AFortKickassGameMode::AFortKickassGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// Use our PlayerState so each player carries a replicated resource count.
	PlayerStateClass = AFortPlayerState::StaticClass();
}
