// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemDataAsset.h"

UItemDataAsset::UItemDataAsset()
{
	// 기본값 설정
	ItemID = NAME_None;
	ItemName = TEXT("Unknown Item");
	Description = TEXT("No description available.");
	SocketName = TEXT("hand_r");
	Scale = FVector(1.0f, 1.0f, 1.0f);
	BaseDamage = 10.0f;
	AttackSpeed = 1.0f;
}

FPrimaryAssetId UItemDataAsset::GetPrimaryAssetId() const
{
	// ItemID를 에셋 ID로 사용
	return FPrimaryAssetId(GetClass()->GetFName(), ItemID);
}