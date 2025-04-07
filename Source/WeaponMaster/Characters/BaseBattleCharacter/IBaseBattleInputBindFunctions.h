// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IBaseBattleInputBindFunctions.generated.h"

struct FInputActionValue;
/**
 * 
 */
UINTERFACE(BlueprintType)
class WEAPONMASTER_API UBaseBattleInputBindFunctions : public UInterface
{
	GENERATED_BODY()
};

class WEAPONMASTER_API IBaseBattleInputBindFunctions
{
	GENERATED_BODY()
public:
	/** Called for movement input */
	virtual void WeakAttack(const FInputActionValue& Value) = 0;
	virtual void StrongAttack(const FInputActionValue& Value) = 0;
	virtual void Identity(const FInputActionValue& Value) = 0;
	virtual void Defence(const FInputActionValue& Value) = 0;
	virtual void PickingItem(const FInputActionValue& Value) = 0;
	virtual void MenuOnOff(const FInputActionValue& Value) = 0;
};
