// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ISkillSystemUser.generated.h"

/**
 * 
 */
UINTERFACE(BlueprintType)
class WEAPONMASTER_API USkillSystemUser : public UInterface
{
	GENERATED_BODY()
};

class WEAPONMASTER_API ISkillSystemUser
{
	GENERATED_BODY()

public:
	// SkillComponent에서 Character에 접근할 메서드
	// ItemComponent에서 Character에 접근할 메서드
	// SkillComponent에서 ITemComponent에 접근할 메서드
	// ItemComponent에서 SkillComponent에 접근할 메서드
};
