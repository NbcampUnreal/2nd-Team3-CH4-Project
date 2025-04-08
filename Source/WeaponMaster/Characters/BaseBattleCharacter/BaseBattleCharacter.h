// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IBaseBattleInputBindFunctions.h"
#include "SSTCharacter.h"
#include "Characters/Components/IBattleSystemUser.h"
#include "BaseBattleCharacter.generated.h"

UCLASS()
class WEAPONMASTER_API ABaseBattleCharacter :
public ASSTCharacter, public IBaseBattleInputBindFunctions, public IBattleSystemUser
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseBattleCharacter(const FObjectInitializer& ObjectInitializer);

protected:
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
	virtual UItemComponent* GetItemComponent() const;

	UFUNCTION(BlueprintCallable, Category = "Items")
	virtual bool EquipItem(FName ItemID);

	// Character -> SillComponent
	UFUNCTION(BlueprintCallable, Category = "Components")
	virtual USkillComponent* GetSkillComponent() const;

	UFUNCTION(BlueprintCallable, Category = "Skills")
	virtual void ExecuteSkill(int32 SkillIndex);

	// ItemComponent -> Character -> SkillComponent
	UFUNCTION(BlueprintCallable, Category = "Items")
	virtual void OnItemEquipped(UItemDataAsset* EquippedItem);

	UFUNCTION(BlueprintCallable, Category = "Items")
	virtual void OnItemUnequipped();

	UFUNCTION(BlueprintCallable, Category = "Skills")
	virtual void InterruptActiveSkill();

	// Interactable Actors
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual void SetInteractableActor(AActor* NewInteractableActor);

	UFUNCTION(BlueprintPure, Category = "Interaction")
	virtual AActor* GetInteractableActor() const;
};
