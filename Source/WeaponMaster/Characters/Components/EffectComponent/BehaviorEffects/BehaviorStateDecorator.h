// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/StatusTypes.h"
#include "../IEffect.h"
#include "Characters/Components/StateComponent/States/CharacterBehaviorState.h"
#include "BehaviorStateDecorator.generated.h"

/**
 * 
 */
UCLASS()
class WEAPONMASTER_API UBehaviorStateDecorator : public UCharacterBehaviorState, public IEffect
{
	GENERATED_BODY()

protected:
	FTimerHandle DurationTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	EBehaviorEffect BehaviorEffectType;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsActive;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	TObjectPtr<UCharacterBehaviorState> InnerState;

public:
	virtual void Activate() override;
	virtual void Activate(float Duration) override;
	virtual void Deactivate() override;
	virtual void Tick(float DeltaTime) override;

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
	virtual void MenuOnOff() override;
	
	FORCEINLINE virtual UCharacterBehaviorState* GetInnerState() { return InnerState; };
	FORCEINLINE virtual void SetInnerState(UCharacterBehaviorState* NewInnerState) { InnerState = NewInnerState; };
};
