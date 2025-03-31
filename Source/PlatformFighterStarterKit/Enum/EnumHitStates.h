#pragma once

#include "CoreMinimal.h"
#include "EnumHitStates.generated.h"

UENUM(BlueprintType)
enum class EHitStates : uint8
{
	Normal UMETA(DisplayName = "Normal"),
	Intangible UMETA(DisplayName = "Intangible"),
	Invincible UMETA(DisplayName = "Invincible"),
	Armor UMETA(DisplayName = "Armor"),
	Shield UMETA(DisplayName = "Shield")
};
