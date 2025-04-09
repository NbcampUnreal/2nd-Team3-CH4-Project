#include "WeaponMaster/Skills/BossAreaSkill.h"
#include "WeaponMaster/Characters/TestCharacter.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimMontage.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "WeaponMaster/Data/StatusTypes.h"

UBossAreaSkill::UBossAreaSkill()
{
    DefaultAttackSpeed = 0.8f;
}

void UBossAreaSkill::ExecuteSkill()
{
    Super::ExecuteSkill();
}

int32 UBossAreaSkill::ProcessTargetActors(const TArray<AActor*>& TargetActors, float Damage)
{
    // 기본 유효성 검사
    if (!OwnerCharacter || !OwnerCharacter->HasAuthority())
    {
        return 0;
    }

    int32 ProcessedCount = 0;

    for (AActor* Target : TargetActors)
    {
        if (!Target || Target == OwnerCharacter)
        {
            continue;
        }
        UE_LOG(LogTemp, Warning, TEXT("[ComboSkill] Hit Target: %s"), *Target->GetName());
        float FinalDamage = SkillDamage;

        // 장비 또는 스킬 계수 적용
        if (ItemData)
        {
            FinalDamage += ItemData->BaseDamage;
        }

        // 데미지 타입 설정
        TSubclassOf<UDamageType> DamageTypeClass = DamageType;
        if (!DamageTypeClass)
        {
            DamageTypeClass = UDamageType::StaticClass();
        }

        UE_LOG(LogTemp, Warning, TEXT("[ComboSkill] Hit Target: %s | Damage: %.1f"), *Target->GetName(), FinalDamage);

        // 실제 데미지 적용
        // Target->TakeDamage(FinalDamage, FDamageEvent(DamageTypeClass), OwnerCharacter->GetController(), OwnerCharacter);

        // 넉백 (선택)
        /* if (KnockbackPower > 0.f)
        {
            ACharacter* TargetChar = Cast<ACharacter>(Target);
            if (TargetChar)
            {
                FVector Dir = (TargetChar->GetActorLocation() - OwnerCharacter->GetActorLocation()).GetSafeNormal();
                Dir.Z = 0.4f;
                TargetChar->LaunchCharacter(Dir * KnockbackPower, true, true);
            }
        }*/

        // 타격 이펙트 (선택)
        if (!SkillEffect.IsNull())
        {
            UNiagaraSystem* Effect = SkillEffect.LoadSynchronous();
            if (Effect)
            {
                UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                    OwnerCharacter->GetWorld(),
                    Effect,
                    Target->GetActorLocation(),
                    OwnerCharacter->GetActorRotation()
                );
            }
        }
        ProcessedCount++;
    }
    return ProcessedCount;
}