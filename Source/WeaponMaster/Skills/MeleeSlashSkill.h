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
    
	// BaseSkill 오버라이드
	virtual void ExecuteSkill() override;

	virtual int32 ProcessTargetActors(const TArray<AActor*>& TargetActors) override;
	
protected:
	// 넉백 파워
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Combat")
	float KnockbackPower;
    
	// 콤보 인덱스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Combat")
	int32 ComboIndex;
    
	// 데미지 지연 시간 (초)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Timing")
	float DamageDelay;
    
private:
	// 타이머 핸들
	FTimerHandle DamageTimerHandle;
};