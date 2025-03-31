#pragma once

#include "CoreMinimal.h"
#include "Shape.generated.h"

UENUM(BlueprintType)
enum class EShape : uint8
{
	Sphere UMETA(DisplayName = "Sphere"),
	Box UMETA(DisplayName = "Box"),
	Pill UMETA(DisplayName = "Pill")
};
