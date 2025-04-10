#include "MeleeSlashSkill.h"
#include "GameFramework/Character.h"
#include "WeaponMaster/Characters/Components/IBattleSystemUser.h"
#include "WeaponMaster/Data/ItemDataAsset.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "TimerManager.h"
#include "Characters/Components/DamageSystemUser.h"
#include "Engine/DamageEvents.h"
#include "Engine/World.h"
#include "WeaponMaster/Characters/Components/ItemComponent/ItemComponent.h"
#include "WeaponMaster/Data/StatusTypes.h"

UMeleeSlashSkill::UMeleeSlashSkill()
{
    // 스킬 기본 정보 설정
    SkillID = TEXT("MeleeSlash");
    SkillName = TEXT("베기 공격");
    Description = TEXT("전방에 있는 적을 베는 근접 공격입니다. 콤보로 연속 사용 가능합니다.");
    SkillType = ESkillType::Melee;
    
    // 스킬 속성 설정
    CooldownTime = 0.5f; // 짧은 쿨다운
    SkillDuration = 0.0f; // 몽타주로 지속시간 제어
    SkillDamage = 15.0f; // 기본 데미지
    CCEffect = ECCSkillCategory::None;
    CCDuration = 0.0f;
    
    // 특화 속성 설정
    KnockbackPower = 300.0f;
    ComboIndex = 0;
    DamageDelay = 0.3f; // 데미지가 적용되기까지의 지연 시간
}

void UMeleeSlashSkill::ExecuteSkill()
{
    Super::ExecuteSkill();
    
}

int32 UMeleeSlashSkill::ProcessTargetActors(const TArray<AActor*>& TargetActors, float Damage)
{
    Super::ProcessTargetActors(TargetActors, Damage);
    // 기본 검증
    if (!OwnerCharacter || !OwnerCharacter->HasAuthority())
    {
        return 0;
    }
    
    int32 ProcessedCount = 0;
    
    // 모든 타겟 액터에 효과 적용
    for (AActor* TargetActor : TargetActors)
    {
        // 액터 유효성 검사
        if (!TargetActor || TargetActor == OwnerCharacter)
        {
            continue;
        }
        
        // 데미지 적용
        float FinalDamage = SkillDamage;
        
        // ItemData에서 추가 데미지 계산 (아이템 데미지 계수)
        // 새로운 헬퍼 메서드 사용
        FinalDamage += GetItemBaseDamage();
        
        if (auto CastedTarget = Cast<IDamageSystemUser>(TargetActor))
        {
            UE_LOG(LogTemp, Display, TEXT("UBossBasicComboSkill::ProcessTargetActors : Target Cast Success!"));
            FVector LaunchVector = {1000.0f, 0.0f, 0.0f};
			
            FAttackData AttackData
            {
                FinalDamage,
                LaunchVector,
                { EBehaviorEffect::Stun, EBehaviorEffect::Confused, EBehaviorEffect::Silence },
                { 1, 5, 3 },
                {},
                {}
            };
			
            CastedTarget->OnAttacked(AttackData);
        }
        else
        {
            UE_LOG(LogTemp, Display, TEXT("UBossBasicComboSkill::ProcessTargetActors : Target Cast Failed!"));
        }
        
        // 타격 이펙트 생성 (선택적)
        if (!SkillEffect.IsNull())
        {
            UNiagaraSystem* ImpactEffect = SkillEffect.LoadSynchronous();
            if (ImpactEffect)
            {
                UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                    OwnerCharacter->GetWorld(),
                    ImpactEffect,
                    TargetActor->GetActorLocation(),
                    OwnerCharacter->GetActorRotation()
                );
            }
        }
        ProcessedCount++;
    }
    
    return ProcessedCount;
}