// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AISpawner.generated.h"

UCLASS()
class WEAPONMASTER_API AAISpawner : public AActor
{
	GENERATED_BODY()
	
protected:
    AAISpawner();

    virtual void BeginPlay() override;

    // 스폰 가능한 AI 클래스 목록
    UPROPERTY(EditAnywhere, Category = "Spawner")
    TArray<TSubclassOf<APawn>> AIClasses;

    // 스폰 간격
    UPROPERTY(EditAnywhere, Category = "Spawner")
    float SpawnInterval = 5.f;

    FTimerHandle SpawnTimer;

    void SpawnAI();
};
