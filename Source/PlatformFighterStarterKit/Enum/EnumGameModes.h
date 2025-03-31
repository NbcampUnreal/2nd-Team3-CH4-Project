#pragma once

#include "CoreMinimal.h"
#include "EnumGameModes.generated.h"

UENUM(BlueprintType)
enum class EGameModes : uint8
{
	VS UMETA(DisplayName = "VS"),
	Online UMETA(DisplayName = "Online"),
	Arcade UMETA(DisplayName = "Arcade")
};
