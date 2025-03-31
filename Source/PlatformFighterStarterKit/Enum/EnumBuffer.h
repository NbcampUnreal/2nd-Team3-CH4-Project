#pragma once

#include "CoreMinimal.h"
#include "EnumBuffer.generated.h"

UENUM(BlueprintType)
enum class EBuffer : uint8
{
	None UMETA(DisplayName = "None"),
	Attack UMETA(DisplayName = "Attack"),
	Special UMETA(DisplayName = "Special"),
	Jump UMETA(DisplayName = "Jump"),
	Sheild UMETA(DisplayName = "Sheild")
};