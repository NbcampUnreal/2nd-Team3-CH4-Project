// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AISpawner.h"
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
    if (AIClasses.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("AIClasses가 비어 있습니다."));
        return;
    }

    // 랜덤 AI 선택
    int32 RandomAIIndex = FMath::RandRange(0, AIClasses.Num() - 1);
    TSubclassOf<APawn> AIToSpawn = AIClasses[RandomAIIndex];

    if (AIToSpawn)
    {
        FActorSpawnParameters SpawnParams;
        FVector SpawnLocation = GetActorLocation();          // Spawner의 위치에서 생성
        FRotator SpawnRotation = GetActorRotation();         // Spawner의 방향

        APawn* SpawnedAI = GetWorld()->SpawnActor<APawn>(AIToSpawn, SpawnLocation, SpawnRotation, SpawnParams);

        if (SpawnedAI)
        {
            UE_LOG(LogTemp, Log, TEXT("AI 스폰 성공: %s"), *SpawnedAI->GetName());
        }
    }
}