// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathEffect.h"

UDeathEffect::UDeathEffect()
{
	BehaviorEffectType = EBehaviorEffect::Death;
	bIsActive = false;
	OuterState = nullptr;
	InnerState = nullptr;
}

void UDeathEffect::Activate()
{
	Super::Activate();
}

void UDeathEffect::Activate(float Duration)
{
	Super::Activate(Duration);
}

void UDeathEffect::Deactivate()
{
	Super::Deactivate();
}

void UDeathEffect::Move(const FInputActionValue& Value)
{
}

void UDeathEffect::CrouchDrop()
{
	UE_LOG(LogTemp, Display, TEXT("UDeathEffect::CrouchDrop"));
}

void UDeathEffect::StopCrouchDrop()
{
}

void UDeathEffect::JumpOrDrop()
{
}

void UDeathEffect::ReleaseJump()
{
}

void UDeathEffect::Dash()
{
}

void UDeathEffect::WeakAttack()
{
}

void UDeathEffect::StrongAttack()
{
}

void UDeathEffect::Identity()
{
}

void UDeathEffect::Defence()
{
}

void UDeathEffect::PickingItem()
{
}