// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IBaseBattleInputBindFunctions.generated.h"

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
	virtual void WeakAttack() = 0;
	virtual void StrongAttack() = 0;
	virtual void Identity() = 0;
	virtual void Defence() = 0;
	virtual void PickingItem() = 0;
};
