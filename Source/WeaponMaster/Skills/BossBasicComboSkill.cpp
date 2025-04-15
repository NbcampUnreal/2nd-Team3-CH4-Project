#include "WeaponMaster/Skills/BossBasicComboSkill.h"
#include "WeaponMaster/Characters/TestCharacter.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimMontage.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Characters/Components/DamageSystemUser.h"
#include "Data/StatusTypes.h"
#include "WeaponMaster/Data/StatusTypes.h"

UBossBasicComboSkill::UBossBasicComboSkill()
{
	SkillDamage = 5.0f;
}

void UBossBasicComboSkill::ExecuteSkill()
{
	Super::ExecuteSkill();
}

int32 UBossBasicComboSkill::ProcessTargetActors(const TArray<AActor*>& TargetActors, float Damage)
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
		float FinalDamage = SkillDamage;

		// 장비 또는 스킬 계수 적용
		if (ItemData)
		{
			FinalDamage += ItemData->BaseDamage;
		}
		if (auto CastedTarget = Cast<IDamageSystemUser>(Target))
		{
			//FVector LaunchVector = {1000.0f, 0.0f, 0.0f};
			UE_LOG(LogTemp, Display, TEXT("UBossBasicComboSkill::ProcessTargetActors : Target Cast Success!"));
			FVector LaunchDirection = OwnerCharacter->GetActorForwardVector();
			FVector LaunchVector = { LaunchDirection.X * 1000, 0.f, 100.f };
			
			FAttackData AttackData
			{
				// FinalDamage,
				OwnerCharacter,
				10.0f,
				LaunchVector,
				{ EBehaviorEffect::Stiffness },
				{ 1 },
				{},
				{}
			};
			
			CastedTarget->OnAttacked(AttackData);
		}
		else
		{
		}

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
