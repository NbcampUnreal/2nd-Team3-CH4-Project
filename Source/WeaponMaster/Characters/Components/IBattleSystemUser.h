// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IBattleSystemUser.generated.h"

class UItemDataAsset;
/**
 * 
 */
UINTERFACE(BlueprintType, NotBlueprintable)
class WEAPONMASTER_API UBattleSystemUser : public UInterface
{
	GENERATED_BODY()
};

class WEAPONMASTER_API IBattleSystemUser
{
	GENERATED_BODY()

public:
	// Character -> ItemComponent
	UFUNCTION(BlueprintCallable, Category = "Components")
	virtual UItemComponent* GetItemComponent() const = 0;

	UFUNCTION(BlueprintCallable, Category = "Items")
	virtual bool EquipItem(FName ItemID) = 0;

	// Character -> SillComponent
	UFUNCTION(BlueprintCallable, Category = "Components")
	virtual USkillComponent* GetSkillComponent() const = 0;

	UFUNCTION(BlueprintCallable, Category = "Skills")
	virtual void ExecuteSkill(int32 SkillIndex) = 0;

	// ItemComponent -> Character -> SkillComponent
	UFUNCTION(BlueprintCallable, Category = "Items")
	virtual void OnItemEquipped(UItemDataAsset* EquippedItem) = 0;

	UFUNCTION(BlueprintCallable, Category = "Items")
	virtual void OnItemUnequipped() = 0;

	UFUNCTION(BlueprintCallable, Category = "Skills")
	virtual void InterruptActiveSkill() = 0;

	// Interactable Actors
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual void SetInteractableActor(AActor* NewInteractableActor) = 0;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual AActor* GetInteractableActor() const = 0;
};
