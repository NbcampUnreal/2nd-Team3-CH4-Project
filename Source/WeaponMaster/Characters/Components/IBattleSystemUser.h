// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Data/StatusTypes.h"
#include "IBattleSystemUser.generated.h"

class USkillComponent;
class UItemComponent;
class UItemDataAsset;
/**
 * 
 */
UINTERFACE(BlueprintType)
class WEAPONMASTER_API UBattleSystemUser : public UInterface
{
	GENERATED_BODY()
};

class WEAPONMASTER_API IBattleSystemUser
{
	GENERATED_BODY()

public:
	// Character -> ItemComponent
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Components")
	UItemComponent* GetItemComponent() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Items")
	bool EquipItem(FName ItemID);

	// Character -> SillComponent
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Components")
	USkillComponent* GetSkillComponent() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Skills")
	void ExecuteSkill(int32 SkillIndex);

	// ItemComponent -> Character -> SkillComponent
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Items")
	void OnItemEquipped(UItemDataAsset* EquippedItem);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Items")
	void OnItemUnequipped();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Skills")
	void InterruptActiveSkill();

	// Character -> StateComponent
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "States")
	TScriptInterface<UBehaviorState> GetBehaviorState() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "States")
	void SetBehaviorState(const TScriptInterface<UBehaviorState>& NewState);
	
	
	// Interactable Actors
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	void SetInteractableActor(AActor* NewInteractableActor);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	AActor* GetInteractableActor() const;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Animation")
	void PlaySkillMontage(UAnimMontage* Montage, float PlayRate);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Animation")
	void SetupMontageEndedDelegate();
};
