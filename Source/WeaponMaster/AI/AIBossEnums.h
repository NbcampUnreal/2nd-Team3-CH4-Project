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

	MeleeBasic        UMETA(DisplayName = "기본 근접 공격"),
	BackTurnCharge    UMETA(DisplayName = "뒤로 후퇴"),
	ThrowObject       UMETA(DisplayName = "무언가 던지기"),

	MeleeHeavy        UMETA(DisplayName = "강한 근접 공격"),
	AOEStomp          UMETA(DisplayName = "전조 범위기"),
	RushHit           UMETA(DisplayName = "몸통박치기"),

	MeteorSmash       UMETA(DisplayName = "무기 낙하 공격"),
	RunThroughAttack  UMETA(DisplayName = "경로 돌진")
};