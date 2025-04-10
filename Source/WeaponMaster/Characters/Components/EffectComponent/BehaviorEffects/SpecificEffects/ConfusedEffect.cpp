// Fill out your copyright notice in the Description page of Project Settings.

#include "ConfusedEffect.h"
#include "InputActionValue.h"


UConfusedEffect::UConfusedEffect()
{
	BehaviorEffectType = EBehaviorEffect::Confused;
	bIsActive = false;
	OuterState = nullptr;
	InnerState = nullptr;
}

void UConfusedEffect::Activate()
{
	Super::Activate();
}

void UConfusedEffect::Activate(float Duration)
{
	Super::Activate(Duration);
}

void UConfusedEffect::Deactivate()
{
	Super::Deactivate();
}

// 좌우반전
void UConfusedEffect::Move(const FInputActionValue& Value)
{
	const float FloatValue = Value.Get<float>();
	const float InvertedFloatValue = -FloatValue;
	
	Super::Move(FInputActionValue(InvertedFloatValue));
}

void UConfusedEffect::CrouchDrop()
{
	Super::CrouchDrop();
}

void UConfusedEffect::StopCrouchDrop()
{
	Super::StopCrouchDrop();
}

void UConfusedEffect::JumpOrDrop()
{
	Super::JumpOrDrop();
}

void UConfusedEffect::ReleaseJump()
{
	Super::ReleaseJump();
}

void UConfusedEffect::Dash()
{
	Super::Dash();
}

void UConfusedEffect::WeakAttack()
{
	Super::WeakAttack();
}

void UConfusedEffect::StrongAttack()
{
	Super::StrongAttack();
}

void UConfusedEffect::Identity()
{
	Super::Identity();
}

void UConfusedEffect::Defence()
{
	Super::Defence();
}

void UConfusedEffect::PickingItem()
{
	Super::PickingItem();
}

void UConfusedEffect::MenuOnOff()
{
	// UE_LOG(LogTemp, Display, TEXT("UConfusedEffect::MenuOnOff"));
	Super::MenuOnOff();
}
