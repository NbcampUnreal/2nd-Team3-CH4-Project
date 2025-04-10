// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IBehaviorState.h"
#include "UObject/Object.h"
#include "CharacterBehaviorState.generated.h"

struct FInputActionValue;
/**
 * 
 */
UCLASS()
class WEAPONMASTER_API UCharacterBehaviorState :
public UObject, public IBehaviorState
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	TScriptInterface<UBehaviorState> OuterState;

public:
	// ISSTInputBindFunctions
	virtual void Move(const FInputActionValue& Value) override;
	virtual void CrouchDrop() override;
	virtual void StopCrouchDrop() override;
	virtual void JumpOrDrop() override;
	virtual void ReleaseJump() override;
	virtual void Dash() override;

	// IBaseBattleInputBindFunctions
	virtual void WeakAttack() override;
	virtual void StrongAttack() override;
	virtual void Identity() override;
	virtual void Defence() override;
	virtual void PickingItem() override;
	virtual void MenuOnOff() override;

	virtual TScriptInterface<UBehaviorState> GetOuterState() const override;
	virtual void SetOuterState(const TScriptInterface<UBehaviorState>& NewOuterState) override;
	
};
