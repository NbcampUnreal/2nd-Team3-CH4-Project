// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IBehaviorState.generated.h"

struct FInputActionValue;
/**
 *  
 */
UINTERFACE(BlueprintType)
class WEAPONMASTER_API UBehaviorState : public UInterface
{
	GENERATED_BODY()
};

class WEAPONMASTER_API IBehaviorState
{
	GENERATED_BODY()

public:
	/** Called for movement input */
	virtual void WeakAttack() = 0;
	virtual void StrongAttack() = 0;
	virtual void Identity() = 0;
	virtual void Defence() = 0;
	virtual void PickingItem() = 0;
	virtual void MenuOnOff() = 0;

	/** Called for movement input */
	virtual void Move(const FInputActionValue& Value) = 0;
	virtual void CrouchDrop() = 0;
	virtual void StopCrouchDrop() = 0;
	virtual void JumpOrDrop() = 0;
	virtual void ReleaseJump() = 0;
	virtual void Dash() = 0;

	// OuterState Getter/Setter
	virtual TScriptInterface<UBehaviorState> GetOuterState() const = 0;
	virtual void SetOuterState(const TScriptInterface<UBehaviorState>& NewOuterState) = 0;
};
