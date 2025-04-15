// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemDataAsset.h"

UItemDataAsset::UItemDataAsset()
{
	// 기본값 초기화
	ItemID = NAME_None;
	ItemName = TEXT("Unknown Item");
	Description = TEXT("No description available.");
	BaseDamage = 10.0f;
	AttackSpeed = 1.0f;
}

FPrimaryAssetId UItemDataAsset::GetPrimaryAssetId() const
{
	// ItemID를 에셋 ID로 사용
	return FPrimaryAssetId(GetClass()->GetFName(), ItemID);
}