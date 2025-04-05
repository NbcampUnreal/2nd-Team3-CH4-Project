#pragma once

#include "CoreMinimal.h"
#include "StatusTypes.h"
#include "Particles/ParticleSystem.h"
#include "SkillTypes.generated.h"

// 스킬 타입 열거형
UENUM(BlueprintType)
enum class ESkillType : uint8
{
    None            UMETA(DisplayName = "None"),
    BasicAttack     UMETA(DisplayName = "Basic Attack"),
    SpecialAttack   UMETA(DisplayName = "Special Attack"),
    Dash            UMETA(DisplayName = "Dash"),
    AreaOfEffect    UMETA(DisplayName = "Area Of Effect"),
    ComboFinisher   UMETA(DisplayName = "Combo Finisher"),
    DefensiveSkill  UMETA(DisplayName = "Defensive Skill"),
    PatternSkill    UMETA(DisplayName = "Pattern Skill")
};

// 입력 액션 타입 열거형
UENUM(BlueprintType)
enum class EInputActionType : uint8
{
    None            UMETA(DisplayName = "None"),
    MoveLeft        UMETA(DisplayName = "Move Left"),
    MoveRight       UMETA(DisplayName = "Move Right"),
    Jump            UMETA(DisplayName = "Jump"),
    Attack          UMETA(DisplayName = "Attack"),
    SpecialAttack   UMETA(DisplayName = "Special Attack"),
    Dodge           UMETA(DisplayName = "Dodge")
};

// 콤보 데이터 구조체 (콤보 단계별 정보)
USTRUCT(BlueprintType)
struct FComboSkillData
{
    GENERATED_BODY()
    
    // 스킬 타입
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
    ESkillType SkillType = ESkillType::None;
    
    // 스킬 이름
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
    FString SkillName = TEXT("None");
    
    // 데미지 배율 (기본 데미지에 곱해질 값)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
    float DamageMultiplier = 1.0f;
    
    // 애니메이션
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    TSoftObjectPtr<UAnimMontage> SkillAnimation;
    
    // 효과음
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    TSoftObjectPtr<USoundBase> SkillSound;
    
    // 파티클 효과
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    TSoftObjectPtr<UParticleSystem> SkillEffect;
};

// 스킬 데이터 구조체
USTRUCT(BlueprintType)
struct FSkillData
{
    GENERATED_BODY()
    
    // 스킬 기본 정보
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    ESkillType SkillType = ESkillType::None;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    FString SkillName = TEXT("None");
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    float Damage = 10.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    float CooldownTime = 0.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    float ResourceCost = 0.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    bool bRequiresTarget = false;
    
    // 상태이상 정보
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effects")
    TArray<ECCSkillCategory> AppliedCC;
    
    // 데미지 타입
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    TSubclassOf<UDamageType> DamageType;
    
    // 애니메이션
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    TSoftObjectPtr<UAnimMontage> SkillAnimation;
    
    // 콤보 정보
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
    TArray<EInputActionType> ComboSequence;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
    float ComboTimeWindow = 1.0f;
    
    // 효과음
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    TSoftObjectPtr<USoundBase> SkillSound;
    
    // 파티클 효과
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    TSoftObjectPtr<UParticleSystem> SkillEffect;
    
    // 콤보 할수 있는 스킬인지 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
    bool bCanCombo = false;
    
    // 콤보의 다음 스킬 데이터 리스트
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
    TArray<FComboSkillData> NextComboSkills;
};

// 스킬 행동 인터페이스
UINTERFACE(MinimalAPI, Blueprintable)
class USkillBehavior : public UInterface
{
    GENERATED_BODY()
};

class ISkillBehavior
{
    GENERATED_BODY()

    /**
     * 지정된 Instigator와 SkillData를 기반으로 스킬을 실행하는 추상 메서드입니다.
     *
     * @param Instigator 스킬을 사용하는 액터입니다.
     * @param SkillData 실행할 스킬의 데이터입니다. 데이터에는 스킬의 타입, 데미지, 쿨다운 시간,
     *                  리소스 비용, 상태 이상 효과 등 다양한 스킬 관련 정보가 포함됩니다.
     */
public:
    virtual void Execute(AActor* Instigator, const FSkillData& SkillData) = 0;
    /**
     * 스킬이 현재 쿨다운 상태인지 확인합니다.
     *
     * @return 스킬이 쿨다운 상태에 있으면 true, 그렇지 않으면 false를 반환합니다.
     */
    virtual bool IsOnCooldown() const = 0;
    /**
     * 현재 남은 쿨다운 시간을 반환합니다.
     *
     * @return float 현재 남은 쿨다운 시간 (초 단위).
     */
    virtual float GetCooldownRemaining() const = 0;
    /**
     * 주어진 시간 동안 스킬의 쿨다운을 적용합니다.
     *
     * @param CooldownTime 스킬 쿨다운에 적용할 시간(초 단위)
     */
    virtual void ApplyCooldown(float CooldownTime) = 0;
};