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

	// 캐릭터 Enum 만들어서 Map으로 관리할까나요?
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TSubclassOf<ACharacter> CharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	int32 TeamID;

	UFUNCTION(BlueprintCallable)
	void Spawn();

	UFUNCTION(BlueprintCallable)
	void SpawnAndPossess(AController* OwnerController);
};
