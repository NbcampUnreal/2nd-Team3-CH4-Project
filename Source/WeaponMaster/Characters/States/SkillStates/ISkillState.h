// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../IState.h"
#include "ISkillState.generated.h"

/**
 * 
 */
UINTERFACE(BlueprintType)
class WEAPONMASTER_API USkillState : public UInterface
{
	GENERATED_BODY()
};

class WEAPONMASTER_API ISkillState : public IState
{
	GENERATED_BODY()
	
};
