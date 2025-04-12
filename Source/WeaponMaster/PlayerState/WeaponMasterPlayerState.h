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
	// 싱글일때 0
	// 협력전일때 0
	// 팀전일때 0, 1
	// 리플리케이트 되도록
	int32 TeamIdx;
	

public:

};
