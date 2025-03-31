#pragma once

#include "CoreMinimal.h"
#include "EnumButtons.generated.h"

UENUM(BlueprintType)
enum class EButtons : uint8
{
	Pressed UMETA(DisplayName = "Pressed"),
	Released UMETA(DisplayName = "Released")
};
