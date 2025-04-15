// Fill out your copyright notice in the Description page of Project Settings.


#include "UpperLanceSlash.h"

#include "NiagaraFunctionLibrary.h"
#include "Characters/Components/DamageSystemUser.h"
#include "Data/ItemDataAsset.h"
#include "Data/StatusTypes.h"
#include "GameFramework/Character.h"

UUpperLanceSlash::UUpperLanceSlash()
{
}

void UUpperLanceSlash::ExecuteSkill()
{
	Super::ExecuteSkill();
}

int32 UUpperLanceSlash::ProcessTargetActors(const TArray<AActor*>& TargetActors, float Damage)
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
			
			UE_LOG(LogTemp, Display, TEXT("UBasicLanceSlash::ProcessTargetActors : Target Cast Success!"));
			FVector LaunchDirection = OwnerCharacter->GetActorForwardVector();
			FVector LaunchVector = { LaunchDirection.X * 100, 0.f, 1500.f };
			
			FAttackData AttackData
			{
				OwnerCharacter,
				FinalDamage,
				LaunchVector,
				{EBehaviorEffect::Stun},
				{1.5f},
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
