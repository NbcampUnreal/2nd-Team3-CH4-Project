// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ISSTInputBindFunctions.h"
#include "IState.h"
#include "Characters/BaseBattleCharacter/IBaseBattleInputBindFunctions.h"
#include "UObject/Object.h"
#include "CharacterBehaviorState.generated.h"

struct FInputActionValue;
/**
 * 
 */
UCLASS()
class WEAPONMASTER_API UCharacterBehaviorState :
public UObject, public IState, public ISSTInputBindFunctions, public IBaseBattleInputBindFunctions
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	TObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	TObjectPtr<UCharacterBehaviorState> OuterState;

public:
	void Initialize(ACharacter* NewOwnerCharacter);

	virtual void Enter() override;
	virtual void Exit() override;
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

	FORCEINLINE virtual UCharacterBehaviorState* GetOuterState() { return OuterState; };
	FORCEINLINE virtual void SetOuterState(UCharacterBehaviorState* NewOuterState) { OuterState = NewOuterState; };
};
