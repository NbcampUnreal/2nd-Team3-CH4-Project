// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skills/BaseSkill.h"
#include "ShieldStunAttack.generated.h"

/**
 * 
 */
UCLASS()
class WEAPONMASTER_API UShieldStunAttack : public UBaseSkill
{
	GENERATED_BODY()

public:
	UShieldStunAttack();
	virtual void ExecuteSkill() override;
	virtual int32 ProcessTargetActors(const TArray<AActor*>& TargetActors, float Damage) override;
};
