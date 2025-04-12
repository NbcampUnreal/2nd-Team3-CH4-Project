// Fill out your copyright notice in the Description page of Project Settings.


#include "StiffnessEffect.h"

UStiffnessEffect::UStiffnessEffect()
{
	BehaviorEffectType = EBehaviorEffect::Stiffness;
	bIsActive = false;
	OuterState = nullptr;
	InnerState = nullptr;
}

void UStiffnessEffect::Activate()
{
	Super::Activate();
}

void UStiffnessEffect::Activate(float Duration)
{
	Super::Activate(Duration);
}

void UStiffnessEffect::Deactivate()
{
	Super::Deactivate();
}

void UStiffnessEffect::Move(const FInputActionValue& Value)
{
}

void UStiffnessEffect::CrouchDrop()
{
}

void UStiffnessEffect::StopCrouchDrop()
{
}

void UStiffnessEffect::JumpOrDrop()
{
}

void UStiffnessEffect::ReleaseJump()
{
}

void UStiffnessEffect::Dash()
{
}

void UStiffnessEffect::WeakAttack()
{
}

void UStiffnessEffect::StrongAttack()
{
}

void UStiffnessEffect::Identity()
{
}

void UStiffnessEffect::Defence()
{
}

void UStiffnessEffect::PickingItem()
{
}
