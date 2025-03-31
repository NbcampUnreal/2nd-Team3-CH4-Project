#pragma once

#include "CoreMinimal.h"
#include "CollisionTypes.generated.h"

UENUM(BlueprintType)
enum class ECollisionTypes : uint8
{
	Normal UMETA(DisplayName = "Normal"),
	Sweet UMETA(DisplayName = "Sweet"),
	Sour UMETA(DisplayName = "Sour"),
	Spike UMETA(DisplayName = "Spike"),
	Metior UMETA(DisplayName = "Metior"),
	Multi UMETA(DisplayName = "Multi")
};
