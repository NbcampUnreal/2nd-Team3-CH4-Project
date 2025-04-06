// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WidgetStateData.generated.h"

UENUM(BlueprintType)
enum class EWidgetState : uint8
{
	CharacterChoice UMETA(DisplayName = "Character Choice"),
	WeaponChoice    UMETA(DisplayName = "Weapon Choice")
};

//UCLASS()
//class WEAPONMASTER_API UWidgetStateData : public UObject
//{
//	GENERATED_BODY()
//	
//};
