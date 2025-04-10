// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AISpawner/AISpawner.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "Kismet/KismetMathLibrary.h"

AAISpawner::AAISpawner()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AAISpawner::BeginPlay()
{
    Super::BeginPlay();

    // 일정 시간마다 SpawnAI 호출
    GetWorldTimerManager().SetTimer(SpawnTimer, this, &AAISpawner::SpawnAI, SpawnInterval, true);
}

void AAISpawner::SpawnAI()
{
    if (CurrentSpawnCount >= MaxSpawnCount)
    {
        UE_LOG(LogTemp, Warning, TEXT("최대 소환 수(%d)에 도달. 스폰 중단."), MaxSpawnCount);
        GetWorldTimerManager().ClearTimer(SpawnTimer);
        return;
    }

    if (AIClasses.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("AIClasses가 비어 있습니다."));
        return;
    }

    int32 RandomAIIndex = FMath::RandRange(0, AIClasses.Num() - 1);
    TSubclassOf<APawn> AIToSpawn = AIClasses[RandomAIIndex];

    if (AIToSpawn)
    {
        FActorSpawnParameters SpawnParams;
        FVector SpawnLocation = GetActorLocation();
        FRotator SpawnRotation = GetActorRotation();

        APawn* SpawnedAI = GetWorld()->SpawnActor<APawn>(AIToSpawn, SpawnLocation, SpawnRotation, SpawnParams);

        if (SpawnedAI)
        {
            ++CurrentSpawnCount;
            UE_LOG(LogTemp, Log, TEXT("AI 스폰 성공 (%d/%d): %s"), CurrentSpawnCount, MaxSpawnCount, *SpawnedAI->GetName());
        }
    }
}