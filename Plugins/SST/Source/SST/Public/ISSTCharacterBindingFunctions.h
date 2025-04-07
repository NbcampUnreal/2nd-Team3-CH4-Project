// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ISSTCharacterBindingFunctions.generated.h"

UINTERFACE(BlueprintType)
class SST_API USSTCharacterBindingFunctions : public UInterface
{
	GENERATED_BODY()
};

class SST_API ISSTCharacterBindingFunctions
{
	GENERATED_BODY()

public:
	/** Called for movement input */
	virtual void Move(const struct FInputActionValue& Value) = 0;
	
	/** Called for crouch/drop input */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Input")
	void CrouchDrop();
	
	/** Called when releasing crouch/drop input */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Input")
	void StopCrouchDrop();
	
	/** Called when jump pressed, which could also be a drop-down command */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Input")
	void JumpOrDrop();
	
	/** Called for dash input */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Input")
	void Dash();

	/** Called when releasing the jump button */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Movement")
	void ReleaseJump();
};
