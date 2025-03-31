#pragma once

#include "CoreMinimal.h"
#include "EnableDisable.generated.h"

UENUM(BlueprintType)
enum class EEnableDisable : uint8
{
	Enabled UMETA(DisplayName = "Enabled"),
	Disabled UMETA(DisplayName = "Disabled"),
	Custom UMETA(DisplayName = "Custom")
};
