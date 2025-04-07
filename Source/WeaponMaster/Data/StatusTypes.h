#pragma once

#include "CoreMinimal.h"
#include "StatusTypes.generated.h"

// 상태이상(CC) 타입 열거형
UENUM(BlueprintType)
enum class ECCSkillCategory : uint8
{
    None        UMETA(DisplayName = "None"),
    Stun        UMETA(DisplayName = "Stun"),
    Knockback   UMETA(DisplayName = "Knockback"),
    Slow        UMETA(DisplayName = "Slow"),
    Root        UMETA(DisplayName = "Root"),
    Burn        UMETA(DisplayName = "Burn"),
    Freeze      UMETA(DisplayName = "Freeze"),
    Poison      UMETA(DisplayName = "Poison"),
    Blind       UMETA(DisplayName = "Blind")
};
