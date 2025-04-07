// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICharacterState.h"
#include "UObject/Interface.h"
#include "ICharacterStateUser.generated.h"

/**
 * 
 */
UINTERFACE(BlueprintType)
class WEAPONMASTER_API UCharacterStateUser : public UInterface
{
	GENERATED_BODY()
};

class WEAPONMASTER_API ICharacterStateUser
{
	GENERATED_BODY()
	virtual void SetState(TObjectPtr<ICharacterState> NextState) = 0;
};
