// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponMasterLib.h"
#include "GameDataManager.h"
#include "Kismet/GameplayStatics.h"


UGameDataManager* UWeaponMasterLib::GetGameDataManager(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		UE_LOG(LogTemp, Warning, TEXT("WorldContextObject is null!"));
		return nullptr;
	}

	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("World is null!"));
		return nullptr;
	}

	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(World);
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameInstance is null!"));
		return nullptr;
	}

	if (GameInstance)
	{
		UGameDataManager* GameDataManager = GameInstance->GetSubsystem<UGameDataManager>();
		if (GameDataManager)
		{
			return GameDataManager;
		}
	}

	return nullptr;
}
