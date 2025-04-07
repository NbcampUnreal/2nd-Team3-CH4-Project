// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponMaster/Skills/BaseSkill.h"
#include "BossAISkill.generated.h"

/**
 * 
 */
UCLASS()
class WEAPONMASTER_API UBossAISkill : public UBaseSkill
{
	GENERATED_BODY()

public:
	UBossAISkill();

	virtual void ExecuteSkill() override;

	virtual int32 ProcessTargetActors(const TArray<AActor*>& TargetActors) override;
};
