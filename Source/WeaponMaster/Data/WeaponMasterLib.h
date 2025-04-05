// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WeaponMasterLib.generated.h"

class UGameDataManager;
/**
 * 
 */
UCLASS()
class WEAPONMASTER_API UWeaponMasterLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	static UGameDataManager* GetGameDataManager(const UObject* WorldContextObject);
};
