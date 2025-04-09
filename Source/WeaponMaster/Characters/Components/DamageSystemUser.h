// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DamageSystemUser.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDamageSystemUser : public UInterface
{
	GENERATED_BODY()
};

class WEAPONMASTER_API IDamageSystemUser
{
	GENERATED_BODY()


public:
	//UFUNCTION(BlueprintCallable, Category = "Attacked")
	//virtual void OnAttacked(float Damage, struct FAttackData AttackData) = 0;
};