// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WeaponMaster/AI/AIComponent/BossStateComponent.h"
#include "BossCharacter.generated.h"

UCLASS()
class WEAPONMASTER_API ABossCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABossCharacter();
protected:
	virtual void BeginPlay() override;
public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Stats")
	float MaxHP;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Stats")
	float CurrentHP;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Component")
	UBossStateComponent* BossStateComponent;

	//UFUNCTION(BlueprintCallable, Category = "Boss|Damage")
	//void TakeBossDamage(float DamageAmount);

	UFUNCTION()
	void OnPhaseChanged(EBossPhase NewPhase);

///보스 공격 패턴
public:
	UFUNCTION(BlueprintCallable)
	void PerformBasicAttack();

	UFUNCTION(BlueprintCallable)
	void ApplyBasicCombo(); // 타격 타이밍에 호출됨

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* BasicAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float AttackRange;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float AttackDamage;

	void PerformComboAttack1();
	void PerformComboAttack2();
	void PerformComboAttack3();

	void CalculateAttackBox(int32 ComboStep, FVector& OutCenter, FVector& OutExtent);
	void DamageActorsInBox(const FVector& Center, const FVector& Extent);
};
