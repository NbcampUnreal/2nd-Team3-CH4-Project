#pragma once

#include "CoreMinimal.h"
#include "EnumDirections.generated.h"

UENUM(BlueprintType)
enum class EDirections : uint8
{
	None UMETA(DisplayName = "None"),
	Up UMETA(DisplayName = "Up"),
	Down UMETA(DisplayName = "Down"),
	Back UMETA(DisplayName = "Back"),
	Forward UMETA(DisplayName = "Forward")
};
