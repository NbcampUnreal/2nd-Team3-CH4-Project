#include "WeaponMaster/Skills/BossAreaSkill.h"
#include "WeaponMaster/Characters/TestCharacter.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimMontage.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Characters/Components/DamageSystemUser.h"
#include "Data/StatusTypes.h"
#include "WeaponMaster/Data/StatusTypes.h"

UBossAreaSkill::UBossAreaSkill()
{
    DefaultAttackSpeed = 0.8f;
	SkillDamage = 50.0f;
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


		if (auto CastedTarget = Cast<IDamageSystemUser>(Target))
		{
			FVector LaunchDirection = OwnerCharacter->GetActorForwardVector();
			FVector LaunchVector = { LaunchDirection.X * 0.f, 0.f, 1500.f };

			FAttackData AttackData
			{
				OwnerCharacter,
				FinalDamage,
				LaunchVector,
				{ EBehaviorEffect::Stun },
				{ 2 },
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