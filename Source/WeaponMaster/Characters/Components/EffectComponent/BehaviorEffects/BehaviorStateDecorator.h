// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/StatusTypes.h"
#include "IBehaviorDecorator.h"
#include "BehaviorStateDecorator.generated.h"

/**
 * 
 */
UCLASS()
class WEAPONMASTER_API UBehaviorStateDecorator : public UObject, public IBehaviorDecorator
{
	GENERATED_BODY()

protected:
	FTimerHandle DurationTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	EBehaviorEffect BehaviorEffectType;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsActive;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	TScriptInterface<UBehaviorState> OuterState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	TScriptInterface<UBehaviorState> InnerState;

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
	
	virtual TScriptInterface<UBehaviorState> GetInnerState() const override;
	virtual void SetInnerState(const TScriptInterface<UBehaviorState>& NewInnerState) override;

	virtual TScriptInterface<UBehaviorState> GetOuterState() const override;
	virtual void SetOuterState(const TScriptInterface<UBehaviorState>& NewOuterState) override;
};
