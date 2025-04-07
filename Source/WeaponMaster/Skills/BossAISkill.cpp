
#include "WeaponMaster/Skills/BossAISkill.h"

UBossAISkill::UBossAISkill()
{

}

void UBossAISkill::ExecuteSkill()
{
    Super::ExecuteSkill();
}

int32 UBossAISkill::ProcessTargetActors(const TArray<AActor*>& TargetActors)
{
    Super::ProcessTargetActors(TargetActors);

    int ProcessedCount = 0;

    return ProcessedCount;
}