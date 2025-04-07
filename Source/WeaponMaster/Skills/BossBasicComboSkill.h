#pragma once

#include "CoreMinimal.h"
#include "WeaponMaster/Skills/BaseSkill.h"
#include "BossBasicComboSkill.generated.h"


UCLASS()
class WEAPONMASTER_API UBossBasicComboSkill : public UBaseSkill
{
	GENERATED_BODY()
public:
	UBossBasicComboSkill();

	virtual int32 ProcessTargetActors(const TArray<AActor*>& TargetActors, float Damage) override;

};
