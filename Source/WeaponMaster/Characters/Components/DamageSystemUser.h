// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Data/StatusTypes.h"
#include "DamageSystemUser.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType, NotBlueprintable)
class UDamageSystemUser : public UInterface
{
	GENERATED_BODY()
};

class WEAPONMASTER_API IDamageSystemUser
{
	GENERATED_BODY()

public:
	// Called When Attacked by Other Character
	UFUNCTION(BlueprintCallable, Category = "Attacked")
	virtual void OnAttacked(const FAttackData& AttackData) = 0;
};