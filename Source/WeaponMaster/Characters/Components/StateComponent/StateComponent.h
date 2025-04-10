// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ISSTInputBindFunctions.h"
#include "Characters/BaseBattleCharacter/IBaseBattleInputBindFunctions.h"
#include "States/IBehaviorState.h"
#include "Components/ActorComponent.h"
#include "States/CharacterBehaviorState.h"
#include "StateComponent.generated.h"

DECLARE_DELEGATE(FOnStateComponentReady);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class WEAPONMASTER_API UStateComponent : public UActorComponent, public ISSTInputBindFunctions, public IBaseBattleInputBindFunctions
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UStateComponent();

	FOnStateComponentReady OnStateComponentReady;

	bool bIsComponentReady;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	TScriptInterface<UBehaviorState> CharacterBehaviorState;

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
	
	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	FORCEINLINE TScriptInterface<UBehaviorState> GetBehaviorState() const { return CharacterBehaviorState; };
	FORCEINLINE void SetBehaviorState(const TScriptInterface<UBehaviorState>& NewState) { CharacterBehaviorState = NewState; };
};
