// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IState.h"
#include "UObject/Interface.h"
#include "IStateUser.generated.h"

/**
 * 
 */
UINTERFACE(BlueprintType)
class WEAPONMASTER_API UStateUser : public UInterface
{
	GENERATED_BODY()
};

class WEAPONMASTER_API IStateUser
{
	GENERATED_BODY()
	virtual void SetState(TObjectPtr<IState> NextState) = 0;
};
