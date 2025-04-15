// Fill out your copyright notice in the Description page of Project Settings.


#include "ShieldStunAttack.h"

#include "NiagaraFunctionLibrary.h"
#include "Characters/Components/DamageSystemUser.h"
#include "Data/StatusTypes.h"
#include "GameFramework/Character.h"

UShieldStunAttack::UShieldStunAttack()
{
}

void UShieldStunAttack::ExecuteSkill()
{
	Super::ExecuteSkill();
}

int32 UShieldStunAttack::ProcessTargetActors(const TArray<AActor*>& TargetActors, float Damage)
{
	// 기본 검증
	if (!OwnerCharacter || !OwnerCharacter->HasAuthority())
	{
		return 0;
	}
    
	int32 ProcessedCount = 0;
	
	for (AActor* TargetActor : TargetActors)
	{
		// 액터 유효성 검사
		if (!TargetActor || TargetActor == OwnerCharacter)
		{
			continue;
		}
        
		// 데미지 적용
		float FinalDamage = SkillDamage + Damage;
		
		FinalDamage += GetItemBaseDamage();
        
		if (auto CastedTarget = Cast<IDamageSystemUser>(TargetActor))
		{
			UE_LOG(LogTemp, Display, TEXT("[UShieldStunAttack::ProcessTargetActors] : Target Cast Success!"));
			FVector LaunchDirection = OwnerCharacter->GetActorForwardVector();
			FVector LaunchVector = { LaunchDirection.X * 500, 0.f, 100.f };
			
			FAttackData AttackData
			{
				OwnerCharacter,
				FinalDamage,
				LaunchVector,
				{ EBehaviorEffect::Stun },
				{ 2.f },
				{},
				{}
			};
			
			CastedTarget->OnAttacked(AttackData);
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("[UShieldStunAttack::ProcessTargetActors] : Target Cast Failed!"));
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
