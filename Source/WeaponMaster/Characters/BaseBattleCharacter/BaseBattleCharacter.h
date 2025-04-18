﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IBaseBattleInputBindFunctions.h"
#include "Characters/Components/StateComponent/States/IBehaviorState.h"
#include "Data/StatusTypes.h"
#include "SSTCharacter.h"
#include "Characters/Components/DamageSystemUser.h"
#include "Characters/Components/IBattleSystemUser.h"
#include "Characters/Components/DebuffWidgetUser.h"
#include "BaseBattleCharacter.generated.h"

class UStateComponent;
class UEffectComponent;
class USkillComponent;
class UWidgetComponent;
class UBaseSkill;

UCLASS()
class WEAPONMASTER_API ABaseBattleCharacter :
public ASSTCharacter, public IBaseBattleInputBindFunctions, public IBattleSystemUser, public IDamageSystemUser, public IDebuffWidgetUser
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseBattleCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	// Components
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStateComponent> StateComponent;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UEffectComponent> EffectComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UItemComponent> ItemComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkillComponent> SkillComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> WidgetComponent;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Interacts", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AActor> InteractableActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ACharacter> LastAttacker;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Battle", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> DebuffWidgetClass;
	
	FTimerHandle LastAttackerTimerHandle;
	FTimerHandle RespawnDelayTimerHandle;
	
	// Constants
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	float MaxHP;

	// Variables
	UPROPERTY(ReplicatedUsing=OnRep_HP, VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float HP;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Appearance", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UTexture2D> CharacterThumbnail;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// PlayerState Setting Timing
	virtual void OnRep_PlayerState() override;

	// Replicate Setting
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_HP();

	// Additional Input Binding Functions
	virtual void WeakAttack() override;
	virtual void StrongAttack() override;
	virtual void Identity() override;
	virtual void Defence() override;
	virtual void PickingItem() override;

	UFUNCTION()
	void OnSkillStarted(UBaseSkill* Skill);
	UFUNCTION()
	void OnSkillEnded(UBaseSkill* Skill);
	
	// UI Input Binding Function
	void MenuOnOff();

	void BindInputFunctions();
	
	void SetHP(float NewHP);
	
	UFUNCTION(Server, Reliable)
	void ServerSetHP(float NewHP);

	void OnDeath();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnDeath();

	UFUNCTION(BlueprintImplementableEvent, Category = "Death")
	void PlayDeathSound();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE UEffectComponent* GetEffectComponent() const { return EffectComponent; };

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
	virtual UStateComponent* GetStateComponent_Implementation() const override;
	
	virtual TScriptInterface<UBehaviorState> GetBehaviorState_Implementation() const override;

	virtual void SetBehaviorState_Implementation(const TScriptInterface<UBehaviorState>& NewState) override;

	// ItemComponent -> Character -> SkillComponent
	virtual void OnItemEquipped_Implementation(UItemDataAsset* EquippedItem) override;

	UFUNCTION()
	void OnItemEquippedForBinding(UItemDataAsset* EquippedItem);
	
	virtual void OnItemUnequipped_Implementation() override;

	UFUNCTION()
	void OnItemUnequippedForBinding();
	
	virtual void InterruptActiveSkill_Implementation() override;

	// Interactable Actors
	virtual void SetInteractableActor_Implementation(AActor* NewInteractableActor) override;
	
	virtual AActor* GetInteractableActor_Implementation() const override;

	// Event when Attacked
	virtual void OnAttacked(const FAttackData& AttackData) override;

	virtual void PlaySkillMontage_Implementation(UAnimMontage* Montage, float PlayRate) override;
	virtual void SetupMontageEndedDelegate_Implementation() override;
	virtual void RequestItemPickup_Implementation(AActor* ItemActor) override;
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayMontage(UAnimMontage* Montage, float PlayRate);

	UFUNCTION()
	void OnLocalMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION(Server, Reliable)
	void Server_RequestItemPickup(AActor* ItemActor);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
	UTexture2D* GetCharacterThumbnail() const;

	/** 현재 HP 반환 */
	UFUNCTION(BlueprintCallable, Category = "Stats")
	float GetHP() const { return HP; }

	/** 최대 HP 반환 */
	UFUNCTION(BlueprintCallable, Category = "Stats")
	float GetMaxHP() const { return MaxHP; }

	// WidgetComponent의 상태이상 위젯 컨트롤 함수들
	virtual void UpdateDebuffWidget() override;
	
	UFUNCTION()
	void SetDebuffWidgetPlayerName();

	UFUNCTION(Client, Reliable)
	void ClientSetDebuffWidgetPlayerName();
};
