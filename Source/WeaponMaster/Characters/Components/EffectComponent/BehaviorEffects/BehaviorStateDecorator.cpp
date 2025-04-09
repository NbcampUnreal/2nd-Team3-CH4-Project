// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviorStateDecorator.h"

void UBehaviorStateDecorator::Activate()
{
}

void UBehaviorStateDecorator::Activate(float Duration)
{
}

void UBehaviorStateDecorator::Deactivate()
{
}

void UBehaviorStateDecorator::Tick(float DeltaTime)
{
}

void UBehaviorStateDecorator::Move(const FInputActionValue& Value)
{
	InnerState->Move(Value);
}

void UBehaviorStateDecorator::CrouchDrop()
{
	InnerState->CrouchDrop();
}

void UBehaviorStateDecorator::StopCrouchDrop()
{
	InnerState->StopCrouchDrop();
}

void UBehaviorStateDecorator::JumpOrDrop()
{
	InnerState->JumpOrDrop();
}

void UBehaviorStateDecorator::ReleaseJump()
{
	InnerState->ReleaseJump();
}

void UBehaviorStateDecorator::Dash()
{
	InnerState->Dash();
}

void UBehaviorStateDecorator::WeakAttack()
{
	InnerState->WeakAttack();
}

void UBehaviorStateDecorator::StrongAttack()
{
	InnerState->StrongAttack();
}

void UBehaviorStateDecorator::Identity()
{
	InnerState->Identity();
}

void UBehaviorStateDecorator::Defence()
{
	InnerState->Defence();
}

void UBehaviorStateDecorator::PickingItem()
{
	InnerState->PickingItem();
}

void UBehaviorStateDecorator::MenuOnOff()
{
	InnerState->MenuOnOff();
}
