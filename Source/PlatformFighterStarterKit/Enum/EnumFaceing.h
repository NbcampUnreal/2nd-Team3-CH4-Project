#pragma once

#include "CoreMinimal.h"
#include "EnumFaceing.generated.h"

UENUM(BlueprintType)
enum class EFaceing : uint8
{
	Left UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right")
};
