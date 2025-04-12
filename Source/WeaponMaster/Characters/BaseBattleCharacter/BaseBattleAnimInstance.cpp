// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseBattleAnimInstance.h"
#include "BaseBattleCharacter.h"
#include "SSTCharacterMovementComponent.h"
#include "Characters/Components/EffectComponent/EffectComponent.h"


UBaseBattleAnimInstance::UBaseBattleAnimInstance()
{
	Character = nullptr;
	MovementComponent = nullptr;
	Velocity = FVector::ZeroVector;
	GroundSpeed = 0.0f;
	bShouldMove = false;
	bIsFalling = false;
	bIsDashing = false;
	bIsCrouched = false;
	bIsWallSliding = false;
	bIsFacingRight = true;
}

void UBaseBattleAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (!IsValid(Character)) return;
	
	auto EffectComponent = Character->GetEffectComponent();
	ActiveBehaviorEffects = EffectComponent->GetActiveBehaviorEffects();

	// Set Velocity and Ground Speed
	Velocity = MovementComponent->Velocity;
	GroundSpeed = Velocity.Size2D();

	// Set bShouldMove When Acceleration > 0 and GroundSpeed > 3.0(Little Threshold)
	bShouldMove = !MovementComponent->GetCurrentAcceleration().IsNearlyZero() && GroundSpeed > 3.0f;

	// Set States From Character Movement Component
	bIsFalling = MovementComponent->IsFalling();
	bIsCrouched = MovementComponent->IsCrouching();

	// Set States From SST Character Movement Component
	bIsWallSliding = MovementComponent->IsWallsliding;
	bIsFacingRight = MovementComponent->IsFacingRight();
	bIsDashing = MovementComponent->IsDashing;
}


