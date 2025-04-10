// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Components/IBattleSystemUser.h"
#include "Characters/Components/StateComponent/States/IBehaviorState.h"
#include "GameFramework/Character.h"
#include "WeaponMaster/AI/AIComponent/BossStateComponent.h"
#include "WeaponMaster/Characters/Components/SkillComponent/SkillComponent.h"
#include "WeaponMaster/Characters/Components/DamageSystemUser.h"
#include "BossCharacter.generated.h"

UCLASS()
class WEAPONMASTER_API ABossCharacter : public ACharacter, public IBattleSystemUser, public IDamageSystemUser
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

	UFUNCTION(BlueprintCallable)
	void Die();

	//보스 몽타주
	UPROPERTY(EditAnywhere, Category = "Death|Montage")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, Category = "RushCharge|Montage")
	UAnimMontage* ForwardChargeMontage;

	UPROPERTY(EditAnywhere, Category = "AreaCharge|Montage")
	UAnimMontage* AreaChargeMontage;
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
	void ApplyForwardCharge();

	void PerformForwardCharge();

	//////2페이즈//////

	//광역기
	UFUNCTION(BlueprintCallable)
	void ApplyAreaSkill();

	void ExecuteAreaSkill();

	//강공격
	UFUNCTION(BlueprintCallable)
	void ApplyPowerAttack();

	// !~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!
	// !~!~!~!~ Battle System User Interface ~!~!~!~!
	// !~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!

	// Character -> ItemComponent
	virtual UItemComponent* GetItemComponent_Implementation() const override;
	
	virtual bool EquipItem_Implementation(FName ItemID) override;
	
	// Character -> SillComponent
	virtual USkillComponent* GetSkillComponent_Implementation() const override;
	
	virtual void ExecuteSkill_Implementation(int32 SkillIndex) override;
	
	// Character -> StateComponent
	virtual TScriptInterface<UBehaviorState> GetBehaviorState_Implementation() const override;
	
	// ItemComponent -> Character -> SkillComponent
	virtual void OnItemEquipped_Implementation(UItemDataAsset* EquippedItem) override;
	
	virtual void OnItemUnequipped_Implementation() override;
	
	virtual void InterruptActiveSkill_Implementation() override;
	
	// Interactable Actors
	virtual void SetInteractableActor_Implementation(AActor* NewInteractableActor) override;
	
	virtual AActor* GetInteractableActor_Implementation() const override;
	
	// Event when Attacked
	virtual void OnAttacked(const FAttackData& AttackData) override;
};
