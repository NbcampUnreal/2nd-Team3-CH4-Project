// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AICharacter/AIBaseBattleCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "WeaponMaster/Data/ItemDataAsset.h"
#include "Characters/Components/EffectComponent/EffectComponent.h"

AAIBaseBattleCharacter::AAIBaseBattleCharacter(const FObjectInitializer& ObjectInitializer)	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.f, 1.f, 0.f));

	GetCharacterMovement()->bOrientRotationToMovement = false;
	bUseControllerRotationYaw = true;
}

void AAIBaseBattleCharacter::BeginPlay()
{
	Super::BeginPlay();
	RandomItemEquip();
	UE_LOG(LogTemp, Warning, TEXT("[AI] 랜덤 아이템 장착 호출"));
}

void AAIBaseBattleCharacter::RandomItemEquip()
{
	UE_LOG(LogTemp, Warning, TEXT("[AI] 랜덤 아이템 장착 하는중"));
	if (ItemAssets.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemAssets 배열이 비어 있습니다."));
	}
	int32 RandIndex = FMath::RandRange(0, ItemAssets.Num() - 1);
	UItemDataAsset* SelectedItem = ItemAssets[RandIndex];

	if (SelectedItem)
	{
		FName ItemID = SelectedItem->ItemID;
		EquipItem_Implementation(ItemID);
		UE_LOG(LogTemp, Warning, TEXT("랜덤 장착 성공: %s"), *ItemID.ToString());
	}
}


void AAIBaseBattleCharacter::ApplyWeakAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("[AI] ApplyWeakAttack 호출"));
	IBattleSystemUser::Execute_ExecuteSkill(this, 0);
}

void AAIBaseBattleCharacter::ApplyStrongAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("[AI] ApplyStrongAttack 호출"));
	IBattleSystemUser::Execute_ExecuteSkill(this, 1);
}

void AAIBaseBattleCharacter::ApplyIdentity()
{
	UE_LOG(LogTemp, Warning, TEXT("[AI] ApplyIdentity 호출"));
	IBattleSystemUser::Execute_ExecuteSkill(this, 2);
}

void AAIBaseBattleCharacter::LookAtTarget(const AActor* TargetActor)
{
	if (!TargetActor) return;

	FVector ToTarget = TargetActor->GetActorLocation() - GetActorLocation();
	ToTarget.Z = 0.f; // 수평 회전만

	if (!ToTarget.IsNearlyZero())
	{
		FRotator LookAtRotation = ToTarget.Rotation();
		SetActorRotation(LookAtRotation);
	}
}

bool AAIBaseBattleCharacter::IsCanToAct() const
{
	if (!EffectComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("EffectComponent가 없습니다."));
		return false;
	}

	const auto& Effects = EffectComponent->GetActiveBehaviorEffects();

	for (auto Debuff : Effects)
	{
		UE_LOG(LogTemp, Warning, TEXT("[IsAbleToAct] 현재 디버프: %s"), *StaticEnum<EBehaviorEffect>()->GetNameStringByValue((uint8)Debuff));
	}

	bool bCanAct = !(Effects.Contains(EBehaviorEffect::Death) ||
		Effects.Contains(EBehaviorEffect::Stun) ||
		Effects.Contains(EBehaviorEffect::Stiffness));

	UE_LOG(LogTemp, Warning, TEXT("[IsAbleToAct] 최종 결과: %s"), bCanAct ? TEXT("True") : TEXT("False"));

	return bCanAct;
}


void AAIBaseBattleCharacter::OnAttacked(const FAttackData& AttackData)
{
	Super::OnAttacked(AttackData);

	if (HP <= 0.f)
	{
		Die();
	}
}

void AAIBaseBattleCharacter::Die()
{
	UE_LOG(LogTemp, Warning, TEXT("죽었습니다."));
	if (EffectComponent->GetActiveBehaviorEffects().Contains(EBehaviorEffect::Death))
	{
		return;
	}
	if (SpawnerOwner)
	{
		FTimerHandle RespawnTimer;
		GetWorld()->GetTimerManager().SetTimer(
			RespawnTimer,
			FTimerDelegate::CreateUObject(SpawnerOwner, &AAISpawner::SpawnAI),
			5.0f, // 5초 후 재소환
			false
		);
	}
}
