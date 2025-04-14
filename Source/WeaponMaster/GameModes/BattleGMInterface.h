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
	virtual void SpawnPlayerCharacter(const TSubclassOf<class ACharacter> CharacterClass, APlayerController* Controller) = 0;
	virtual void HandlePlayerDeath(const TSubclassOf<ACharacter>& CharacterClass, APlayerController* Controller, const FString& AttackerName) = 0;
};
