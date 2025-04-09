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
		UE_LOG(LogTemp, Error, TEXT("has no authority or No OwnerCharacter"));
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
		if (auto CastedTarget = Cast<IDamageSystemUser>(Target))
		{
			UE_LOG(LogTemp, Display, TEXT("UBossBasicComboSkill::ProcessTargetActors : Target Cast Success!"));
			FVector LaunchVector = {1000.0f, 0.0f, 0.0f};
			
			FAttackData AttackData
			{
				Damage,
				LaunchVector,
				{ EBehaviorEffect::Stun },
				{ 1 },
				{},
				{}
			};
			
			CastedTarget->OnAttacked(AttackData);
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("UBossBasicComboSkill::ProcessTargetActors : Target Cast Failed!"));
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
