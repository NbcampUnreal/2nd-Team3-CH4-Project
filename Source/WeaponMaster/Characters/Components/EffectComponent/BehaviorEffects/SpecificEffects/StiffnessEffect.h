// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Components/EffectComponent/BehaviorEffects/BehaviorStateDecorator.h"
#include "StiffnessEffect.generated.h"

/**
 * 
 */
UCLASS()
class WEAPONMASTER_API UStiffnessEffect : public UBehaviorStateDecorator
{
	GENERATED_BODY()

public:
	UStiffnessEffect();
	
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
