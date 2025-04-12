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

	/** 팀 ID 반환 */
	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	uint8 GetTeamID() const { return TeamIdx; }

	/** 팀 ID 설정 */
	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void SetTeamID(uint8 NewTeamID) { TeamIdx = NewTeamID; }
};
