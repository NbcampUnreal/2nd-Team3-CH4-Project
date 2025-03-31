#pragma once

#include "CoreMinimal.h"
#include "AbilityTypes.generated.h"

UENUM(BlueprintType)
enum class EAbilityTypes : uint8
{
	None UMETA(DisplayName = "None"),
	Basic UMETA(DisplayName = "Basic"),
	Special UMETA(DisplayName = "Special"),
	Air UMETA(DisplayName = "Air"),
	Smash UMETA(DisplayName = "Smash"),
	Super UMETA(DisplayName = "Super"),
	Dodge UMETA(DisplayName = "Dodge"),
	Taunt UMETA(DisplayName = "Taunt"),
	Ledge UMETA(DisplayName = "Ledge"),
	Throw UMETA(DisplayName = "Throw"),
	Prone UMETA(DisplayName = "Prone"),
	Grab UMETA(DisplayName = "Grab"),
	Other UMETA(DisplayName = "Other")
};
