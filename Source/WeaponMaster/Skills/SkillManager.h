#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WeaponMaster/Data/SkillTypes.h"
#include "SkillManager.generated.h"

class UCollisionManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnComboProgressed, int32, ComboCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnComboReset);

UCLASS()
class WEAPONMASTER_API USkillManager : public UObject, public ISkillBehavior
{
    GENERATED_BODY()
    
public:
    USkillManager();
    
    // ISkillBehavior 인터페이스 구현
    virtual void Execute(AActor* Instigator, const FSkillData& SkillData) override;
    virtual bool IsOnCooldown() const override;
    virtual float GetCooldownRemaining() const override;
    virtual void ApplyCooldown(float CooldownTime) override;
    
    /** 스킬 등록 */
    UFUNCTION(BlueprintCallable, Category = "Skills")
    void RegisterSkill(const FSkillData& SkillData);
    
    /** 스킬 실행 (인덱스 기반) */
    UFUNCTION(BlueprintCallable, Category = "Skills")
    void ExecuteSkill(int32 SkillIndex, AActor* Instigator);
    
    /** 인덱스로 스킬 데이터 가져오기 */
    UFUNCTION(BlueprintPure, Category = "Skills")
    FSkillData GetSkillData(int32 SkillIndex) const;
    
    /** 인덱스로 스킬이 쿨다운 중인지 확인 */
    UFUNCTION(BlueprintPure, Category = "Skills")
    bool IsSkillOnCooldown(int32 SkillIndex) const;
    
    /** 인덱스로 스킬의 남은 쿨다운 시간 가져오기 */
    UFUNCTION(BlueprintPure, Category = "Skills")
    float GetCooldownRemaining(int32 SkillIndex) const;
    
    /** 무기 충돌 처리 */
    UFUNCTION()
    void ProcessWeaponCollision(UPrimitiveComponent* Weapon, AActor* HitActor, const FHitResult& HitResult);
    
protected:
    /** 스킬 데이터 배열 (인덱스 기반) */
    UPROPERTY(VisibleAnywhere, Category = "Skills")
    TArray<FSkillData> SkillDataArray;
    
    /** 인덱스 기반 쿨다운 타이머 핸들 배열 */
    TArray<FTimerHandle> CooldownTimerHandles;
    
    /** 현재 활성화된 스킬 인덱스 */
    int32 ActiveSkillIndex;
    
    /** 충돌 관리자 */
    UPROPERTY()
    UCollisionManager* CollisionManager;
    
    /** 애니메이션 처리 */
    void PlaySkillAnimation(AActor* Instigator, const FSkillData& SkillData);
    
    /** 충돌 활성화 상태 */
    bool bCollisionEnabled;
};