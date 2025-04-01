// Copyright Epic Games, Inc. All Rights Reserved.

#include "WeaponMasterGameMode.h"
#include "WeaponMasterCharacter.h"
#include "UObject/ConstructorHelpers.h"

AWeaponMasterGameMode::AWeaponMasterGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
