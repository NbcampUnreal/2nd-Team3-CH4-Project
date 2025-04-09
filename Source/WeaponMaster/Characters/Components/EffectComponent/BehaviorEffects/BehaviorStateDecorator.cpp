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
	Cast<IBehaviorState>(InnerState.GetObject())->Move(Value);
}

void UBehaviorStateDecorator::CrouchDrop()
{
	Cast<IBehaviorState>(InnerState.GetObject())->CrouchDrop();
}

void UBehaviorStateDecorator::StopCrouchDrop()
{
	Cast<IBehaviorState>(InnerState.GetObject())->StopCrouchDrop();
}

void UBehaviorStateDecorator::JumpOrDrop()
{
	Cast<IBehaviorState>(InnerState.GetObject())->JumpOrDrop();
}

void UBehaviorStateDecorator::ReleaseJump()
{
	Cast<IBehaviorState>(InnerState.GetObject())->ReleaseJump();
}

void UBehaviorStateDecorator::Dash()
{
	Cast<IBehaviorState>(InnerState.GetObject())->Dash();
}

void UBehaviorStateDecorator::WeakAttack()
{
	Cast<IBehaviorState>(InnerState.GetObject())->WeakAttack();
}

void UBehaviorStateDecorator::StrongAttack()
{
	Cast<IBehaviorState>(InnerState.GetObject())->StrongAttack();
}

void UBehaviorStateDecorator::Identity()
{
	Cast<IBehaviorState>(InnerState.GetObject())->Identity();
}

void UBehaviorStateDecorator::Defence()
{
	Cast<IBehaviorState>(InnerState.GetObject())->Defence();
}

void UBehaviorStateDecorator::PickingItem()
{
	Cast<IBehaviorState>(InnerState.GetObject())->PickingItem();
}

void UBehaviorStateDecorator::MenuOnOff()
{
	Cast<IBehaviorState>(InnerState.GetObject())->MenuOnOff();
}

TScriptInterface<UBehaviorState> UBehaviorStateDecorator::GetInnerState() const
{
	return InnerState;
}

void UBehaviorStateDecorator::SetInnerState(const TScriptInterface<UBehaviorState>& NewInnerState)
{
	InnerState = NewInnerState;
}

TScriptInterface<UBehaviorState> UBehaviorStateDecorator::GetOuterState() const
{ 
	return OuterState;
}

void UBehaviorStateDecorator::SetOuterState(const TScriptInterface<UBehaviorState>& NewOuterState)
{
	OuterState = NewOuterState;
}

