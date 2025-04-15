// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skills/BaseSkill.h"
#include "IdentityLance.generated.h"

/**
 * 
 */
UCLASS()
class WEAPONMASTER_API UIdentityLance : public UBaseSkill
{
	GENERATED_BODY()

public:
	UIdentityLance();
	virtual void ExecuteSkill() override;
	virtual int32 ProcessTargetActors(const TArray<AActor*>& TargetActors, float Damage) override;
};
