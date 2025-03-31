#pragma once

#include "CoreMinimal.h"
#include "EnumCharacter.generated.h"

UENUM(BlueprintType)
enum class ECharacter : uint8
{
	None UMETA(DisplayName = "None"),
	Fighter UMETA(DisplayName = "Fighter"),
	SwordMan UMETA(DisplayName = "SwordMan"),
	Gunner UMETA(DisplayName = "Gunner"),
	Hats UMETA(DisplayName = "Hats"),
	Mage UMETA(DisplayName = "Mage"),
	Random UMETA(DisplayName = "Random")
};
