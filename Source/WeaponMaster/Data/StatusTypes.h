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
    Silence     UMETA(DisplayName = "Silence"),
    Confused    UMETA(DisplayName = "Confused"),
    // AirBorn     UMETA(DisplayName = "AirBorn"),
    
    UsingSkill  UMETA(DisplayName = "UsingSkill"),
    Death       UMETA(DisplayName = "Death"),
    Max         UMETA(DisplayName = "Max") // int32 NumEffects = static_cast<uint8>(ENonBehaviorEffect::Max);
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

/** 플레이어 상태 정보 구조체 */
USTRUCT(BlueprintType)
struct FPlayerStatusInfo
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Status")
    FString PlayerName;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Status")
    float CurrentHealth;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Status")
    float MaxHealth;
    
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Status")
    UTexture2D* PlayerThumbnailTexture;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Status")
    int32 TeamID;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Status")
    int32 CharacterID;
    
    // 생성자로 기본값 설정
    FPlayerStatusInfo()
        : PlayerName(TEXT("Unknown")),
          CurrentHealth(100.0f),
          MaxHealth(100.0f),
          PlayerThumbnailTexture(nullptr),
          TeamID(0),
          CharacterID(-1)
    {
    }
};

USTRUCT(BlueprintType)
struct FAttackData
{
    GENERATED_BODY()

    // 어태커
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ACharacter* Attacker; 
    
    // 데미지
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Damage;

    // 날려보내기 벡터
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector LaunchVector;

    // 입력 상태이상
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<EBehaviorEffect> BehaviorEffects;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<float> BehaviorEffectsDurations;

    // 기타 버프/디버프
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<EBehaviorEffect> NonBehaviorEffects;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<float> NonBehaviorEffectsDurations;
};