// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IEffect.generated.h"

/**
 *  
 */
UINTERFACE(BlueprintType)
class WEAPONMASTER_API UEffect : public UInterface
{
	GENERATED_BODY()
};

class WEAPONMASTER_API IEffect
{
	GENERATED_BODY()

public:
	virtual void Activate() = 0;
	virtual void Activate(float Duration) = 0;
	virtual void Deactivate() = 0;
	virtual void Tick(float DeltaTime) = 0;
};
