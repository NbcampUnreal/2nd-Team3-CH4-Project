#pragma once

#include "CoreMinimal.h"
#include "BaseSkill.h"
#include "MeleeSlashSkill.generated.h"

/**
 * 근접 베기 스킬 예제 클래스
 */
UCLASS()
class WEAPONMASTER_API UMeleeSlashSkill : public UBaseSkill
{
	GENERATED_BODY()

public:
	UMeleeSlashSkill();
    
	// 효과 파라미터 오버라이드
	virtual FSkillEffectParams GetEffectParams() override;
};