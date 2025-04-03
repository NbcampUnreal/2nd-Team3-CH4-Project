#include "WeaponMaster/AI/AIComponent/BossStateComponent.h"

UBossStateComponent::UBossStateComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UBossStateComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


void UBossStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

