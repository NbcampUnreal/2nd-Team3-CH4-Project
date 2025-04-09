// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WeaponMaster/AI/AIComponent/BossStateComponent.h"
#include "WeaponMaster/Characters/Components/SkillComponent/SkillComponent.h"
#include "WeaponMaster/Characters/Components/DamageSystemUser.h"
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
	int32 MaxHP;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Stats")
	int32 CurrentHP;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Component")
	UBossStateComponent* BossStateComponent;

	//UFUNCTION(BlueprintCallable, Category = "Boss|Damage")
	//void TakeBossDamage(float DamageAmount);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	USkillComponent* SkillComponent;

	UPROPERTY(EditAnywhere, Category = "Boss")
	USkillDataAsset* BossSkillAsset;


	UFUNCTION()
	void OnPhaseChanged(EBossPhase NewPhase);
	//피격 판정
	void OnAttacked(int Damage);

	void Die();
	UPROPERTY(EditAnywhere, Category = "Death|Montage")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, Category = "Charge|Montage")
	UAnimMontage* ChargeMontage;
/// 보스 뒤돌기

public:
	UFUNCTION(BlueprintCallable)
	void LookAtTarget(const AActor* TargetActor);

private:
	FRotator DesiredRotation;
	bool bShouldRotate = false;
	float RotationInterpSpeed = 5.f;

///보스 공격 패턴
public:
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayMontage(UAnimMontage* Montage);

	//////1페이즈//////
	//기본 공격 

	UFUNCTION(BlueprintCallable)
	void ApplyBasicCombo();

	//뒤로 대쉬
	UFUNCTION(BlueprintCallable)
	void ApplyBackStep();

	//앞으로 돌진 공격
	UFUNCTION(BlueprintCallable)
	void ExecuteForwardCharge();

	//////2페이즈//////

	//광역기
	UFUNCTION(BlueprintCallable)
	void ApplyAreaSkill();

	void ExecuteAreaSkill();

	//강공격
	UFUNCTION(BlueprintCallable)
	void ApplyPowerAttack();
};
