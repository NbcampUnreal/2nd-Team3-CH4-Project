#pragma once

#include "CoreMinimal.h"
#include "InGameModes.generated.h"

UENUM(BlueprintType)
enum class EInGameModes : uint8
{
	Stock UMETA(DisplayName = "Stock"),
	Time UMETA(DisplayName = "Time"),
	MiniGame UMETA(DisplayName = "MiniGame"),
	BossFight UMETA(DisplayName = "BossFight")
};
