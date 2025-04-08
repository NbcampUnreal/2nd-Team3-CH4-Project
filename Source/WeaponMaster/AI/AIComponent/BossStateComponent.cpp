#include "WeaponMaster/AI/AIComponent/BossStateComponent.h"

UBossStateComponent::UBossStateComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	CurrentPhase = EBossPhase::Phase1; // 기본값
}

void UBossStateComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBossStateComponent::UpdatePhase(float CurrentHP, float MaxHP)
{
	float Ratio = CurrentHP / MaxHP;
	EBossPhase NewPhase = CurrentPhase;

	if (Ratio > 0.7f)
		NewPhase = EBossPhase::Phase1;
	else if (Ratio > 0.3f)
		NewPhase = EBossPhase::Phase2;
	else
		NewPhase = EBossPhase::Phase3;

	if (NewPhase != CurrentPhase)
	{
		CurrentPhase = NewPhase;
		OnPhaseChanged.Broadcast(NewPhase);
	}
}