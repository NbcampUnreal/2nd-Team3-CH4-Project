// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../BehaviorStateDecorator.h"
#include "StunEffect.generated.h"

/**
 * 
 */
UCLASS()
class WEAPONMASTER_API UStunEffect : public UBehaviorStateDecorator
{
	GENERATED_BODY()

public:
	UStunEffect();
	
	virtual void Activate() override;
	virtual void Activate(float Duration) override;
	virtual void Deactivate() override;
	
	virtual void Move(const FInputActionValue& Value) override;
	virtual void CrouchDrop() override;
	virtual void StopCrouchDrop() override;
	virtual void JumpOrDrop() override;
	virtual void ReleaseJump() override;
	virtual void Dash() override;

	virtual void WeakAttack() override;
	virtual void StrongAttack() override;
	virtual void Identity() override;
	virtual void Defence() override;
	virtual void PickingItem() override;
};
