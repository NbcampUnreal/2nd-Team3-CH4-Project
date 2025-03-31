#pragma once

#include "CoreMinimal.h"
#include "Attacks.generated.h"

UENUM(BlueprintType)
enum class EAttacks : uint8
{
	None UMETA(DisplayName = "None"),
	TiltUp UMETA(DisplayName = "TiltUp"),
	TiltDown UMETA(DisplayName = "TiltDown"),
	TiltForward UMETA(DisplayName = "TiltForward"),
	TiltBack UMETA(DisplayName = "TiltBack"),
	Nuetral UMETA(DisplayName = "Nuetral"),
	AirUp UMETA(DisplayName = "AirUp"),
	AirDown UMETA(DisplayName = "AirDown"),
	AirBack UMETA(DisplayName = "AirBack"),
	AirForward UMETA(DisplayName = "AirForward"),
	AirNuetral UMETA(DisplayName = "AirNuetral"),
	SpecialUP UMETA(DisplayName = "SpecialUP"),
	SpecialDown UMETA(DisplayName = "SpecialDown"),
	SpecialForward UMETA(DisplayName = "SpecialForward"),
	SpecialBack UMETA(DisplayName = "SpecialBack"),
	SpecialNuetral UMETA(DisplayName = "SpecialNuetral"),
	SmashUP UMETA(DisplayName = "SmashUP"),
	SmashDown UMETA(DisplayName = "SmashDown"),
	SmashForward UMETA(DisplayName = "SmashForward"),
	SmashBack UMETA(DisplayName = "SmashBack"),
	Grab UMETA(DisplayName = "Grab"),
	ThrowUP UMETA(DisplayName = "ThrowUP"),
	ThrowDown UMETA(DisplayName = "ThrowDown"),
	ThrowForward UMETA(DisplayName = "ThrowForward"),
	ThrowBack UMETA(DisplayName = "ThrowBack"),
	Pumble UMETA(DisplayName = "Pumble"),
	LedgeAttack UMETA(DisplayName = "LedgeAttack"),
	ProneAttack UMETA(DisplayName = "ProneAttack"),
	DodgeSpot UMETA(DisplayName = "DodgeSpot"),
	DodgeAir UMETA(DisplayName = "DodgeAir"),
	DodgeBack UMETA(DisplayName = "DodgeBack"),
	DodgeForward UMETA(DisplayName = "DodgeForward"),
	Super UMETA(DisplayName = "Super")
};