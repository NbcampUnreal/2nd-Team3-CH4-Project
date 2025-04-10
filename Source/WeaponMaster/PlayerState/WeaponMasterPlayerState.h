// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "../UIData/UISelectData.h"
#include "WeaponMasterPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class WEAPONMASTER_API AWeaponMasterPlayerState : public APlayerState
{
	GENERATED_BODY()
protected:
	uint8 TeamIdx;
	
public:
	FUniqueNetIdRepl UniqueId;
	
};
