// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ICharacterState.generated.h"

/**
 *  // Walking, Idle, ... , UseSkill, 피격
 */
UINTERFACE(BlueprintType)
class WEAPONMASTER_API UCharacterState : public UInterface
{
	GENERATED_BODY()
};

class WEAPONMASTER_API ICharacterState
{
	GENERATED_BODY()

public:
	virtual void Enter() = 0;
	virtual void Exit() = 0;
	virtual void Tick(float DeltaTime) = 0;
};
