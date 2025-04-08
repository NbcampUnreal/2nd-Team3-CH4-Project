// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillGroupArray.h"
#include "Engine/DataAsset.h"
#include "SkillDataAsset.generated.h"


/**
 * 
 */
UCLASS()
class WEAPONMASTER_API USkillDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skills")
	TArray<FSkillGroupArray> SkillGroups;
};
