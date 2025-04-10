// Fill out your copyright notice in the Description page of Project Settings.


#include "SilenceEffect.h"

USilenceEffect::USilenceEffect()
{
	BehaviorEffectType = EBehaviorEffect::Silence;
	bIsActive = false;
	OuterState = nullptr;
	InnerState = nullptr;
}

void USilenceEffect::Activate()
{
	Super::Activate();
}

void USilenceEffect::Activate(float Duration)
{
	Super::Activate(Duration);
}

void USilenceEffect::Deactivate()
{
	Super::Deactivate();
}

void USilenceEffect::Move(const FInputActionValue& Value)
{
	Super::Move(Value);
}

void USilenceEffect::CrouchDrop()
{
	Super::CrouchDrop();
}

void USilenceEffect::StopCrouchDrop()
{
	Super::StopCrouchDrop();
}

void USilenceEffect::JumpOrDrop()
{
	Super::JumpOrDrop();
}

void USilenceEffect::ReleaseJump()
{
	Super::ReleaseJump();
}

void USilenceEffect::Dash()
{
}

void USilenceEffect::WeakAttack()
{
}

void USilenceEffect::StrongAttack()
{
}

void USilenceEffect::Identity()
{
}

void USilenceEffect::Defence()
{
}

void USilenceEffect::PickingItem()
{
}

void USilenceEffect::MenuOnOff()
{
	Super::MenuOnOff();
}
