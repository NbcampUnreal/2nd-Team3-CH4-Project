#include "HitReactComponent.h"


UHitReactComponent::UHitReactComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UHitReactComponent::BeginPlay()
{
	Super::BeginPlay();

	
}


void UHitReactComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}


void UHitReactComponent::OnGetDamage()
{
	AActor* Owner = GetOwner();
	if (!Owner) return;
	/*
	AWeaponMasterCharacter Character = Cast<ACharacterBase>(Owner);
	if(Character)
	{
		체력 계산
		if(사망 시)
		{
			상대 점수 1점 획득
			체력 0
			사망상태로 변경
		}
		else
		{
		체력감소
		상태변화
		}
	
	}
	*/ 
	
}