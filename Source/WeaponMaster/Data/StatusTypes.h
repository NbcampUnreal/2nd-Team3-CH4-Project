#pragma once

#include "CoreMinimal.h"
#include "StatusTypes.generated.h"

// 상태이상(CC) 타입 열거형
UENUM(BlueprintType)
enum class ECCSkillCategory : uint8
{
    None        UMETA(DisplayName = "None"),
    Behavior    UMETA(DisplayName = "Behavior"),
    Stat        UMETA(DisplayName = "Stat"),
    Dot         UMETA(DisplayName = "Dot"),
    Max         UMETA(DisplayName = "Max")
};

UENUM(BlueprintType)
enum class EBehaviorEffect : uint8
{
    Stiffness   UMETA(DisplayName = "Stiffness"),
    Stun        UMETA(DisplayName = "Stun"),
    Knockback   UMETA(DisplayName = "Knockback"),
    AirBorn     UMETA(DisplayName = "AirBorn"),
    Blind       UMETA(DisplayName = "Blind"),
    Silence     UMETA(DisplayName = "Silence"),

    UsingSkill  UMETA(DisplayName = "UsingSkill"),
    Death       UMETA(DisplayName = "Death"),
    Max         UMETA(DisplayName = "Max")
};

UENUM(BlueprintType)
enum class ENonBehaviorEffect : uint8
{
    // Soft CC
    Slow        UMETA(DisplayName = "Slow"),
    AttackDown  UMETA(DisplayName = "AttackDown"),

    // Dot Effect
    Burn        UMETA(DisplayName = "Burn"),
    Freeze      UMETA(DisplayName = "Freeze"),
    Poison      UMETA(DisplayName = "Poison"),
    Max         UMETA(DisplayName = "Max")
};


USTRUCT(BlueprintType)
struct FAttackData
{
    GENERATED_BODY()

    // 데미지
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Damage;

    // 날라가는 벡터
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector LaunchVector;

    // 상태이상
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 BehaviorEffectNumbers;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<EBehaviorEffect> BehaviorEffects;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<float> BehaviorEffectsDurations;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 NonBehaviorEffectNumbers;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<EBehaviorEffect> NonBehaviorEffects;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<float> NonBehaviorEffectsDurations;
};