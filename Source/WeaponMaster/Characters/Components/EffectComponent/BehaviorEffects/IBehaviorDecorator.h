// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Components/StateComponent/States/IBehaviorState.h"
#include "IBehaviorDecorator.generated.h"

struct FInputActionValue;
/**
 *  
 */
UINTERFACE(BlueprintType)
class WEAPONMASTER_API UBehaviorDecorator : public UBehaviorState
{
	GENERATED_BODY()
};

class WEAPONMASTER_API IBehaviorDecorator : public IBehaviorState
{
	GENERATED_BODY()

public:
	/** Called for movement input */
	virtual void WeakAttack() override = 0;
	virtual void StrongAttack() override = 0;
	virtual void Identity() override = 0;
	virtual void Defence() override = 0;
	virtual void PickingItem() override = 0;
	virtual void MenuOnOff() override = 0;

	/** Called for movement input */
	virtual void Move(const FInputActionValue& Value) override = 0;
	virtual void CrouchDrop() override = 0;
	virtual void StopCrouchDrop() override = 0;
	virtual void JumpOrDrop() override = 0;
	virtual void ReleaseJump() override = 0;
	virtual void Dash() override = 0;

	// OuterState Getter/Setter
	virtual TScriptInterface<UBehaviorState> GetOuterState() const override = 0;
	virtual void SetOuterState(const TScriptInterface<UBehaviorState>& NewOuterState) override = 0;
	
	// Effect On/Off
	virtual void Activate() = 0;
	virtual void Activate(float Duration) = 0;
	virtual void Deactivate() = 0;
	virtual void Tick(float DeltaTime) = 0;
	
	// InnerState Getter/Setter
	virtual TScriptInterface<UBehaviorState> GetInnerState() const = 0;
	virtual void SetInnerState(TScriptInterface<UBehaviorState> NewInnerState) = 0;
};

