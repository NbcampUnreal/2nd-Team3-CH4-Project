// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ISSTInputBindFunctions.generated.h"

struct FInputActionValue;
/**
 * 
 */
UINTERFACE(BlueprintType)
class SST_API USSTInputBindFunctions : public UInterface
{
	GENERATED_BODY()
};

class SST_API ISSTInputBindFunctions
{
	GENERATED_BODY()
	
public:
	/** Called for movement input */
	virtual void Move(const FInputActionValue& Value) = 0;
	virtual void CrouchDrop() = 0;
	virtual void StopCrouchDrop() = 0;
	virtual void JumpOrDrop() = 0;
	virtual void ReleaseJump() = 0;
	virtual void Dash() = 0;
};
