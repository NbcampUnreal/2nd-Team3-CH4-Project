#pragma once

#include "CoreMinimal.h"
#include "EnumPlayerStates.generated.h"

UENUM(BlueprintType)
enum class EPlayerStates : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	WalkOrRun UMETA(DisplayName = "Walk/Run"),
	Crouch UMETA(DisplayName = "Crouch"),
	Sprint UMETA(DisplayName = "Sprint"),
	Jump UMETA(DisplayName = "Jump"),
	Fall UMETA(DisplayName = "Fall"),
	Hit UMETA(DisplayName = "Hit"),
	Launch UMETA(DisplayName = "Launch"),
	Ledge UMETA(DisplayName = "Ledge"),
	Dead UMETA(DisplayName = "Dead"),
	Shiled UMETA(DisplayName = "Shiled"),
	Tumble UMETA(DisplayName = "Tumble"),
	ShildTumble UMETA(DisplayName = "ShildTumble"),
	Prone UMETA(DisplayName = "Prone"),
	Stun UMETA(DisplayName = "Stun"),
	Dizzy UMETA(DisplayName = "Dizzy"),
	Ability UMETA(DisplayName = "Ability"),
	Dodge UMETA(DisplayName = "Dodge"),
	FreeFall UMETA(DisplayName = "FreeFall"),
	Held UMETA(DisplayName = "Held"),
	Hold UMETA(DisplayName = "Hold")
};
