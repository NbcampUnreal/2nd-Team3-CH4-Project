// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterSpawner.generated.h"

UCLASS()
class WEAPONMASTER_API ACharacterSpawner : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACharacterSpawner();

	UFUNCTION(BlueprintCallable)
	ACharacter* SpawnCharacter(TSubclassOf<ACharacter> CharacterClass);
};
