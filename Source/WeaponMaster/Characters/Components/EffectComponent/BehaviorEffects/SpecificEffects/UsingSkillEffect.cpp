// Fill out your copyright notice in the Description page of Project Settings.

#include "UsingSkillEffect.h"

UUsingSkillEffect::UUsingSkillEffect()
{
	BehaviorEffectType = EBehaviorEffect::UsingSkill;
	bIsActive = false;
	OuterState = nullptr;
	InnerState = nullptr;
}

void UUsingSkillEffect::Activate()
{
	Super::Activate();
}

void UUsingSkillEffect::Activate(float Duration)
{
	Super::Activate(Duration);
}

void UUsingSkillEffect::Deactivate()
{
	Super::Deactivate();
}

void UUsingSkillEffect::Move(const FInputActionValue& Value)
{
	// Super::Move(Value);
}

void UUsingSkillEffect::CrouchDrop()
{
	// Super::CrouchDrop();
}

void UUsingSkillEffect::StopCrouchDrop()
{
	// Super::StopCrouchDrop();
}

void UUsingSkillEffect::JumpOrDrop()
{
	// Super::JumpOrDrop();
}

void UUsingSkillEffect::ReleaseJump()
{
	// Super::ReleaseJump();
}

void UUsingSkillEffect::Dash()
{
	// Super::Dash();
}

void UUsingSkillEffect::WeakAttack()
{
	// Super::WeakAttack();
}

void UUsingSkillEffect::StrongAttack()
{
	// Super::StrongAttack();
}

void UUsingSkillEffect::Identity()
{
	// Super::Identity();
}

void UUsingSkillEffect::Defence()
{
	// Super::Defence();
}

void UUsingSkillEffect::PickingItem()
{
	// Super::PickingItem();
}
