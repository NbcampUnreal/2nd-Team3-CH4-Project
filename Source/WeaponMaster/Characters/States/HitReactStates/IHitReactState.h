// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../IState.h"
#include "IHitReactState.generated.h"

/**
 * 
 */
UINTERFACE(BlueprintType)
class WEAPONMASTER_API UHitReactState : public UInterface
{
	GENERATED_BODY()
};

class WEAPONMASTER_API IHitReactState : public IState
{
	GENERATED_BODY()
	
};
