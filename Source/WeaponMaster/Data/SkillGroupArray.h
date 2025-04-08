#pragma once

#include "CoreMinimal.h"
#include "WeaponMaster/Skills/BaseSkill.h"
#include "SkillGroupArray.generated.h"

/**
 * 스킬 그룹을 저장하는 구조체
 * TArray<TArray<>> 형태가 UPROPERTY로 직접 노출되지 않아 구조체로 래핑
 */
USTRUCT(BlueprintType)
struct FSkillGroupArray
{
	GENERATED_BODY()
    
	// 그룹 내 스킬 클래스 배열
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
	TArray<TSubclassOf<UBaseSkill>> SkillClasses;
};