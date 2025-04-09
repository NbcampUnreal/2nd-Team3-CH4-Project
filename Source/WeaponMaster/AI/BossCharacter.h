// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WeaponMaster/AI/AIComponent/BossStateComponent.h"
#include "WeaponMaster/Characters/Components/SkillComponent/SkillComponent.h"
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	USkillComponent* SkillComponent;

	UPROPERTY(EditAnywhere, Category = "Boss")
	USkillDataAsset* BossSkillAsset;


	UFUNCTION()
	void OnPhaseChanged(EBossPhase NewPhase);
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
	UPROPERTY(EditAnywhere, Category = "Combat|Montage")
	UAnimMontage* ComboMontage1;

	UFUNCTION(BlueprintCallable)
	void ApplyBasicCombo();

	UFUNCTION(BlueprintCallable)
	void StartBasicCombo(); // 타격 타이밍에 호출됨

	UFUNCTION(Server, Reliable)
	void Server_ApplyBasicCombo();

	//1,2,3타
	void PerformComboAttack();


	//뒤로 대쉬

	UPROPERTY(EditAnywhere, Category = "Boss|Movement")
	float DashPower = 1200.f;

	UPROPERTY(EditAnywhere, Category = "Boss|Movement")
	float JumpBoost = 200.f;

	UPROPERTY(EditAnywhere, Category = "Boss|Montage")
	UAnimMontage* BackStepMontage;


	UFUNCTION(BlueprintCallable)
	void ApplyBackStep();

	//앞으로 돌진 공격
	UFUNCTION(BlueprintCallable)
	void ExecuteForwardCharge();

	//////2페이즈//////
	UFUNCTION(BlueprintCallable)
	void ApplyAreaSkill();

	void ExecuteAreaSkill();
};
