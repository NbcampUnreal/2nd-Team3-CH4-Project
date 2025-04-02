// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../IState.h"
#include "ICharacterState.generated.h"

/**
 * 
 */
UINTERFACE(BlueprintType)
class WEAPONMASTER_API UCharacterState : public UInterface
{
	GENERATED_BODY()
};

class WEAPONMASTER_API ICharacterState : public IState
{
	GENERATED_BODY()
};
