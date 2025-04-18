// Fill out your copyright notice in the Description page of Project Settings.

#include "Skills/BossForwardChargeSkill.h"
#include "WeaponMaster/Characters/TestCharacter.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimMontage.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Characters/Components/DamageSystemUser.h"
#include "Data/StatusTypes.h"
#include "WeaponMaster/Data/StatusTypes.h"

UBossForwardChargeSkill::UBossForwardChargeSkill()
{
	DefaultAttackSpeed = 1.0f;
	SkillDamage = 35.0f;
}

void UBossForwardChargeSkill::ExecuteSkill()
{
	Super::ExecuteSkill();
}

int32 UBossForwardChargeSkill::ProcessTargetActors(const TArray<AActor*>& TargetActors, float Damage)
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
		

		UE_LOG(LogTemp, Warning, TEXT("[ComboSkill] Hit Target: %s | Damage: %.1f"), *Target->GetName(), FinalDamage);


		if (auto CastedTarget = Cast<IDamageSystemUser>(Target))
		{
			FVector LaunchDirection = OwnerCharacter->GetActorForwardVector();
			FVector LaunchVector = { LaunchDirection.X * 1500.f, 0.f, 100.f };
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
