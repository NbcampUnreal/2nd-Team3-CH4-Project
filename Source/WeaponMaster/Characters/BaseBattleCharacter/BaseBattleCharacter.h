// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IBaseBattleInputBindFunctions.h"
#include "Data/StatusTypes.h"
#include "SSTCharacter.h"
#include "Characters/Components/IBattleSystemUser.h"
#include "BaseBattleCharacter.generated.h"

class UCharacterBehaviorState;
class USkillComponent;

UCLASS()
class WEAPONMASTER_API ABaseBattleCharacter :
public ASSTCharacter, public IBaseBattleInputBindFunctions, public IBattleSystemUser
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseBattleCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	// Components
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStateComponent> StateComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UEffectComponent> EffectComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UItemComponent> ItemComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USkillComponent> SkillComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Interacts", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AActor> InteractableActor;

	// Constants
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	float MaxHP;

	// Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float HP;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Additional Input Binding Functions
	virtual void WeakAttack() override;
	virtual void StrongAttack() override;
	virtual void Identity() override;
	virtual void Defence() override;
	virtual void PickingItem() override;
	virtual void MenuOnOff() override;

	void BindInputFunctions();

	void SetHP(float NewHP);
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// !~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!
	// !~!~!~!~ Battle System User Interface ~!~!~!~!
	// !~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!
	
	// Character -> ItemComponent
	UFUNCTION(BlueprintCallable, Category = "Components")
	virtual UItemComponent* GetItemComponent() const override;

	UFUNCTION(BlueprintCallable, Category = "Items")
	virtual bool EquipItem(FName ItemID) override;

	// Character -> SillComponent
	UFUNCTION(BlueprintCallable, Category = "Components")
	virtual USkillComponent* GetSkillComponent() const override;

	UFUNCTION(BlueprintCallable, Category = "Skills")
	virtual void ExecuteSkill(int32 SkillIndex) override;

	// Character -> StateComponent
	UFUNCTION(BlueprintCallable, Category = "States")
	virtual UCharacterBehaviorState* GetBehaviorState() const override;

	// ItemComponent -> Character -> SkillComponent
	UFUNCTION(BlueprintCallable, Category = "Items")
	virtual void OnItemEquipped(UItemDataAsset* EquippedItem) override;

	UFUNCTION(BlueprintCallable, Category = "Items")
	virtual void OnItemUnequipped() override;

	UFUNCTION(BlueprintCallable, Category = "Skills")
	virtual void InterruptActiveSkill() override;

	// Interactable Actors
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual void SetInteractableActor(AActor* NewInteractableActor) override;

	UFUNCTION(BlueprintPure, Category = "Interaction")
	virtual AActor* GetInteractableActor() const override;

	// Event when Attacked
	UFUNCTION(BlueprintCallable, Category = "Attacked")
	virtual void OnAttacked(float Damage, const FAttackData& AttackData) override;
};
