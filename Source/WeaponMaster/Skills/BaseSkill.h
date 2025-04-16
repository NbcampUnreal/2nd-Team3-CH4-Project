// BaseSkill.h 리팩토링
#pragma once

#include "CoreMinimal.h"
#include "Data/StatusTypes.h"
#include "UObject/NoExportTypes.h"
#include "GameFramework/DamageType.h"
#include "BaseSkill.generated.h"

class UNiagaraSystem;
class ATestCharacter;
class UItemDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillStartedDelegate, UBaseSkill*, Skill);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillEndedDelegate, UBaseSkill*, Skill);

/**
 * 효과 적용 데이터 구조체
 */
USTRUCT(BlueprintType)
struct FSkillEffectParams
{
    GENERATED_BODY()
    
    // 적용할 행동 효과 목록
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    TArray<EBehaviorEffect> Effects;
    
    // 효과 지속 시간 목록 (Effects 배열과 인덱스 일치)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    TArray<float> Durations;
    
    // 넉백/런치 방향 승수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Launch")
    FVector LaunchDirectionMultiplier = FVector(1000.0f, 0.0f, 100.0f);
    
    // 수직 런치 여부 (true=수직 런치, false=전방 런치)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Launch")
    bool bVerticalLaunch = false;
    
    // 기본 생성자
    FSkillEffectParams()
    {
        // 기본값으로 비어있는 효과와 지속시간 배열
        Effects.Empty();
        Durations.Empty();
    }
};

/**
 * 스킬 타입 열거형
 */
UENUM(BlueprintType)
enum class ESkillType : uint8
{
    Melee       UMETA(DisplayName = "근접 공격"),
    Ranged      UMETA(DisplayName = "원거리 공격"),
    Area        UMETA(DisplayName = "범위 공격"),
    Buff        UMETA(DisplayName = "버프"),
    Movement    UMETA(DisplayName = "이동 기술"),
    Special     UMETA(DisplayName = "특수 기술")
};

/**
 * 스킬 기본 클래스 - 모든 스킬의 부모 클래스
 */
UCLASS(Blueprintable, Abstract)
class WEAPONMASTER_API UBaseSkill : public UObject
{
    GENERATED_BODY()

public:
    UBaseSkill();
    
    /**
     * 스킬 초기화 함수
     * @param Owner 스킬을 소유한 캐릭터
     * @param OwnerItem 스킬을 제공하는 아이템 데이터
     */
    UFUNCTION(BlueprintCallable, Category = "Skill")
    virtual void Initialize(ACharacter* Owner, UItemDataAsset* OwnerItem);
    
    /**
     * 스킬 활성화 시도
     * @return 스킬 활성화 성공 여부
     */
    UFUNCTION(BlueprintCallable, Category = "Skill")
    virtual bool ActivateSkill();
    
    /**
     * 스킬 실행 함수 (자식 클래스에서 실제 스킬 로직 구현)
     */
    UFUNCTION(BlueprintCallable, Category = "Skill")
    virtual void ExecuteSkill();
    
    /**
     * 스킬 종료 함수
     */
    UFUNCTION(BlueprintCallable, Category = "Skill")
    virtual void EndSkill();


    /**
     * 몽타주 종료 시 호출되는 함수
     */
    UFUNCTION()
    virtual void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
    
    /**
     * 스킬 재사용 가능 여부 확인
     * @return 스킬 재사용 가능 여부
     */
    UFUNCTION(BlueprintPure, Category = "Skill")
    bool IsSkillReady() const;
    
    /**
     * 스킬 사용 중인지 확인
     * @return 스킬 사용 중 여부
     */
    UFUNCTION(BlueprintPure, Category = "Skill")
    bool IsSkillActive() const;
    
    /**
     * 스킬 쿨다운 업데이트
     * @param DeltaTime 시간 간격
     */
    UFUNCTION(BlueprintCallable, Category = "Skill")
    void UpdateCooldown(float DeltaTime);
    
    /**
     * 스킬 ID 반환
     * @return 스킬 ID
     */
    UFUNCTION(BlueprintPure, Category = "Skill")
    FName GetSkillID() const { return SkillID; }
    
    /**
     * 스킬 이름 반환
     * @return 스킬 이름
     */
    UFUNCTION(BlueprintPure, Category = "Skill")
    FString GetSkillName() const { return SkillName; }
    
    /**
     * 스킬 타입 반환
     * @return 스킬 타입
     */
    UFUNCTION(BlueprintPure, Category = "Skill")
    ESkillType GetSkillType() const { return SkillType; }
    
    /**
     * 스킬 쿨다운 시간 반환
     * @return 스킬 쿨다운 시간
     */
    UFUNCTION(BlueprintPure, Category = "Skill")
    float GetCooldownTime() const { return CooldownTime; }
    
    /**
     * 현재 쿨다운 남은 시간 반환
     * @return 현재 쿨다운 남은 시간
     */
    UFUNCTION(BlueprintPure, Category = "Skill")
    float GetRemainingCooldown() const { return RemainingCooldown; }
    
    /**
     * 쿨다운 진행률 반환 (0.0-1.0)
     * @return 쿨다운 진행률
     */
    UFUNCTION(BlueprintPure, Category = "Skill")
    float GetCooldownProgress() const;
    
    /**
     * 스킬 아이콘 반환
     * @return 스킬 아이콘 텍스처
     */
    UFUNCTION(BlueprintPure, Category = "Skill")
    TSoftObjectPtr<UTexture2D> GetSkillIcon() const { return SkillIcon; }

    /**
     * 액터 오브젝트 배열을 받아 처리하는 함수
     * @param TargetActors 스킬이 적용될 대상 액터들의 배열
     * @param AdditionalDamage 추가 데미지
     * @return 성공적으로 처리된 액터 수
     */
    UFUNCTION(BlueprintCallable, Category = "Skill")
    virtual int32 ProcessTargetActors(const TArray<AActor*>& TargetActors, float AdditionalDamage = 0.0f);
    
    /**
     * 아이템의 기본 데미지를 안전하게 가져오는 함수
     * ItemData가 없으면 기본값 반환
     * @return 아이템 기본 데미지 또는 기본값
     */
    UFUNCTION(BlueprintPure, Category = "Skill")
    float GetItemBaseDamage() const;
    
    /**
     * 아이템의 공격 속도를 안전하게 가져오는 함수
     * ItemData가 없으면 기본값 반환
     * @return 아이템 공격 속도 또는 기본값
     */
    UFUNCTION(BlueprintPure, Category = "Skill")
    float GetItemAttackSpeed() const;

    /**
     * 단일 대상에 효과 적용하는 공통 로직
     * @param Target 효과를 적용할 대상
     * @param Damage 적용할 데미지
     * @return 적용 성공 여부
     */
    UFUNCTION(BlueprintCallable, Category = "Skill")
    virtual bool ApplyEffectToTarget(AActor* Target, float Damage);

    /**
     * 타겟에 적용할 효과 파라미터 가져오기 (자식 클래스에서 오버라이드)
     * @return 효과 파라미터
     */
    UFUNCTION(BlueprintCallable, Category = "Skill")
    virtual FSkillEffectParams GetEffectParams();

    /**
     * 데미지 계산
     * @param AdditionalDamage 추가 데미지
     * @return 최종 데미지
     */
    UFUNCTION(BlueprintCallable, Category = "Skill")
    virtual float CalculateDamage(float AdditionalDamage = 0.0f);

    /**
     * 이펙트 생성 유틸리티 함수
     * @param Location 이펙트를 생성할 위치 
     * @param Rotation 이펙트 회전
     */
    UFUNCTION(BlueprintCallable, Category = "Skill|Effects")
    void SpawnEffect(const FVector& Location, const FRotator& Rotation);

    virtual bool IsSupportedForNetworking() const override
    {
        return true;
    }

    UFUNCTION()
    virtual void UpdateFromReplicatedData(float NewRemainingCooldown, bool bNewIsActive)
    {
        RemainingCooldown = NewRemainingCooldown;
        bIsActive = bNewIsActive;
    }
    
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    // 스킬 시작 델리게이트
    UPROPERTY(BlueprintAssignable, Category = "Skill")
    FOnSkillStartedDelegate OnSkillStarted;

    // 스킬 종료 델리게이트
    UPROPERTY(BlueprintAssignable, Category = "Skill")
    FOnSkillEndedDelegate OnSkillEnded;

protected:
    // 스킬 ID
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Info")
    FName SkillID;
    
    // 스킬 이름
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Info")
    FString SkillName;
    
    // 스킬 설명
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Info", meta = (MultiLine=true))
    FString Description;
    
    // 스킬 타입
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Info")
    ESkillType SkillType;
    
    // 스킬 쿨다운 시간
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Timing")
    float CooldownTime;
    
    // 스킬 데미지
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Combat")
    float SkillDamage;
    
    // 스킬 몽타주
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Animation")
    TSoftObjectPtr<UAnimMontage> SkillMontage;
    
    // 스킬 이펙트
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|VFX")
    TSoftObjectPtr<UNiagaraSystem> SkillEffect;
    
    // 스킬 사운드
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Audio")
    TSoftObjectPtr<USoundBase> SkillSound;
    
    // 스킬 아이콘
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|UI")
    TSoftObjectPtr<UTexture2D> SkillIcon;
    
    // 효과 파라미터
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Effects")
    FSkillEffectParams EffectParams;
    
    // 소유자 캐릭터
    UPROPERTY(Transient)
    ACharacter* OwnerCharacter;
    
    // 소유 아이템
    UPROPERTY(Transient)
    UItemDataAsset* ItemData;
    
    // 현재 쿨다운 남은 시간
    UPROPERTY(Transient)
    float RemainingCooldown;
    
    // 스킬 활성화 상태
    UPROPERTY(Transient)
    bool bIsActive;
    
    // 내부 타이머 (지속 시간 등 처리)
    UPROPERTY(Transient)
    float SkillTimer;
    
    // 기본 공격 속도 (ItemData가 없을 때 사용)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Defaults")
    float DefaultAttackSpeed;
    
    // 기본 데미지 (ItemData가 없을 때 사용)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Defaults")
    float DefaultBaseDamage;
};