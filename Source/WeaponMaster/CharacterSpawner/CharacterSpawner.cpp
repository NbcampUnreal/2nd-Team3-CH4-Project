// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterSpawner.h"
#include "GameFramework/Character.h"

// Sets default values
ACharacterSpawner::ACharacterSpawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

ACharacter* ACharacterSpawner::SpawnCharacter(TSubclassOf<ACharacter> CharacterClass)
{
	if (!HasAuthority() || !IsValid(CharacterClass) || !IsValid(GetWorld()))
	{
		UE_LOG(LogTemp, Warning, TEXT("ACharacterSpawner::SpawnCharacter : Invalid Calling"));
		return nullptr;
	}

	FTransform SpawnTransform = GetActorTransform();
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;

	ACharacter* SpawnedCharacter = GetWorld()->SpawnActor<ACharacter>(CharacterClass, SpawnTransform, SpawnParams);

	if (!IsValid(SpawnedCharacter))
	{
		UE_LOG(LogTemp, Warning, TEXT("ACharacterSpawner::SpawnCharacter : Failed to spawn character"));
		return nullptr;
	}

	return SpawnedCharacter;
}




