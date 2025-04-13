// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "BattleGMInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UBattleGMInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class WEAPONMASTER_API IBattleGMInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void SetPlayerCharacter(TSubclassOf<class ACharacter> CharacterClass, FName ItemName, AController* OwnerController) = 0;

	virtual void HandlePlayerDeath(AController* Controller) = 0;
};
