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
    SkillID = TEXT("MeleeSlash");
    SkillName = TEXT("베기 공격");
    Description = TEXT("전방에 있는 적을 베는 근접 공격입니다. 콤보로 연속 사용 가능합니다.");
    SkillType = ESkillType::Melee;
    
    // 스킬 속성 설정
    CooldownTime = 0.5f; // 짧은 쿨다운
    SkillDamage = 15.0f; // 기본 데미지
    
    // 효과 파라미터 설정
    EffectParams.Effects = { EBehaviorEffect::Stun, EBehaviorEffect::Stiffness };
    EffectParams.Durations = { 1.0f, 2.0f };
    EffectParams.LaunchDirectionMultiplier = FVector(1000.0f, 0.0f, 100.0f);
    EffectParams.bVerticalLaunch = false;
}

FSkillEffectParams UMeleeSlashSkill::GetEffectParams()
{
    return Super::GetEffectParams();
}
