// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WeaponMaster/Skills/BaseSkill.h"
#include "ItemDataAsset.generated.h"


/**
 * 
 */
UCLASS()
class WEAPONMASTER_API UItemDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UItemDataAsset();
	
	// 아이템 기본 정보
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
	FName ItemID;
    
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
	FString ItemName;
    
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info", meta = (MultiLine=true))
	FString Description;
    
	// 아이템 메시 (월드에 드롭되었을 때)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visuals")
	TSoftObjectPtr<UStaticMesh> ItemMesh;
    
	// 장착했을 때 사용할 메시
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visuals")
	TSoftObjectPtr<UStaticMesh> EquippedMesh;
    
	// 아이템을 부착할 소켓 이름
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attachment")
	FName SocketName;
    
	// 아이템 스케일
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attachment")
	FVector Scale;
    
	// 기본 공격력
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float BaseDamage;
    
	// 공격 속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float AttackSpeed;
    
	// 아이콘 (UI 표시용)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSoftObjectPtr<UTexture2D> Icon;
    
	// 획득 효과음
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	TSoftObjectPtr<USoundBase> PickupSound;
    
	// 장착 효과음
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	TSoftObjectPtr<USoundBase> EquipSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skills")
	TArray<TSubclassOf<UBaseSkill>> SkillClasses;
	
	// DataAsset 타입 오버라이드
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
