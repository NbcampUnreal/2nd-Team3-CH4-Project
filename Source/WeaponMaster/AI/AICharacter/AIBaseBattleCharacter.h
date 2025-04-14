// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseBattleCharacter/BaseBattleCharacter.h"
#include "AI/AISpawner/AISpawner.h"
#include "AIBaseBattleCharacter.generated.h"

UCLASS()
class WEAPONMASTER_API AAIBaseBattleCharacter : public ABaseBattleCharacter
{
	GENERATED_BODY()
public:
	AAIBaseBattleCharacter(const FObjectInitializer& ObjectInitializer);

	UPROPERTY()
	AAISpawner* SpawnerOwner;

// 랜덤 장착용 아이템 목록
	UPROPERTY(EditAnywhere, Category = "AI|Item")
	TArray<UItemDataAsset*> ItemAssets;

	// 랜덤 장착 아이템 장착
	UFUNCTION(BlueprintCallable, Category = "AI|Item")
	void RandomItemEquip();

	//AI의 공격
	UFUNCTION(BlueprintCallable, Category = "AI|Attack")
	void ApplyWeakAttack();

	UFUNCTION(BlueprintCallable, Category = "AI|Attack")
	void ApplyStrongAttack();

	UFUNCTION(BlueprintCallable, Category = "AI|Attack")
	void ApplyIdentity();
private:
	virtual void BeginPlay() override;
};