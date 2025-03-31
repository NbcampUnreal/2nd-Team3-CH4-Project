#pragma once

#include "CoreMinimal.h"
#include "Maps.generated.h"

UENUM(BlueprintType)
enum class EMaps : uint8
{
	ConflictZone UMETA(DisplayName = "Conflict Zone"),
	ConcludingGround UMETA(DisplayName = "Concluding Ground"),
	AnotherLevel UMETA(DisplayName = "Another Level"),
	TheWalkWay UMETA(DisplayName = "TheWalkWay"),
	IceZone UMETA(DisplayName = "IceZone"),
	Random UMETA(DisplayName = "Random")
};
