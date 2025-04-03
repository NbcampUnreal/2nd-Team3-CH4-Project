#pragma once

#include "CoreMinimal.h"
#include "AIBossEnums.generated.h" 


UENUM(BlueprintType)
enum class EBossPhase : uint8
{
	Phase1 UMETA(DisplayName = "Phase 1"),
	Phase2 UMETA(DisplayName = "Phase 2"),
	Phase3 UMETA(DisplayName = "Phase 3")
};

UENUM(BlueprintType)
enum class EBossPattern : uint8
{
	None UMETA(DisplayName = "None"),
	BasicAttack UMETA(DisplayName = "기본공격"),
	Throw UMETA(DisplayName = "던지기"),
	BackCharge UMETA(DisplayName = "뒤로가서 돌진"),
	Stomp UMETA(DisplayName = "범위기"),
	RushBump UMETA(DisplayName = "몸통박치기"),
	Meteor UMETA(DisplayName = "무기 낙하"),
	RunAttack UMETA(DisplayName = "경로 돌진")
};
