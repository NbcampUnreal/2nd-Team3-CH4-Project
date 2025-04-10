// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleGMInterface.h"
#include "GameFramework/GameMode.h"
#include "LSHTestGameMode.generated.h"

/**
 * 
 */
UCLASS()
class WEAPONMASTER_API ALSHTestGameMode : public AGameMode, public IBattleGMInterface
{
	GENERATED_BODY()

public:
	virtual void SetPlayerCharacter(TSubclassOf<ACharacter> CharacterClass, FName ItemName, AController* OwnerController) override;
};
