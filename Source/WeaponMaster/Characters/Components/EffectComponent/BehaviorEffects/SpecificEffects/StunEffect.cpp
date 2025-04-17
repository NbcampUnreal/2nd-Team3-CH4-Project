// Fill out your copyright notice in the Description page of Project Settings.

#include "StunEffect.h"
#include "GameFramework/Character.h"
#include "Characters/Components/IBattleSystemUser.h"
#include "Characters/Components/StateComponent/StateComponent.h"


UStunEffect::UStunEffect()
{
	BehaviorEffectType = EBehaviorEffect::Stun;
	bIsActive = false;
	OuterState = nullptr;
	InnerState = nullptr;
}
 
void UStunEffect::Activate()
{
	Super::Activate();
}

void UStunEffect::Activate(float Duration)
{
	Super::Activate(Duration);
}

void UStunEffect::Deactivate()
{
	Super::Deactivate();
}

// 입력 차단
void UStunEffect::Move(const FInputActionValue& Value)
{
}

void UStunEffect::CrouchDrop()
{
}

void UStunEffect::StopCrouchDrop()
{
}

void UStunEffect::JumpOrDrop()
{
}

void UStunEffect::ReleaseJump()
{
}

void UStunEffect::Dash()
{
}

void UStunEffect::WeakAttack()
{
}

void UStunEffect::StrongAttack()
{
}

void UStunEffect::Identity()
{
}

void UStunEffect::Defence()
{
}

void UStunEffect::PickingItem()
{
}
