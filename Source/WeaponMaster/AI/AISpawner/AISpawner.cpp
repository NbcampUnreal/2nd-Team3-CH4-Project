// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AISpawner/AISpawner.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "AI/AICharacter/AIBaseBattleCharacter.h"

AAISpawner::AAISpawner()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AAISpawner::BeginPlay()
{
    Super::BeginPlay();
    if (HasAuthority())
    {
        GetWorldTimerManager().SetTimer(SpawnTimer, this, &AAISpawner::SpawnAI, 1.5f, false);
    }
}

void AAISpawner::SpawnAI()
{
    if (CurrentSpawnCount >= MaxSpawnCount)
    {
        GetWorldTimerManager().ClearTimer(SpawnTimer);
        return;
    }

    if (AIClasses.Num() == 0)
    {
        return;
    }

    int32 RandomAIIndex = FMath::RandRange(0, AIClasses.Num() - 1);
    TSubclassOf<APawn> AIToSpawn = AIClasses[RandomAIIndex];

    if (AIToSpawn)
    {
        FActorSpawnParameters SpawnParams;
        FVector SpawnLocation = GetActorLocation();
        FRotator SpawnRotation = GetActorRotation();

        APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(AIToSpawn, SpawnLocation, SpawnRotation, SpawnParams);

        if (SpawnedPawn)
        {
            if (AAIBaseBattleCharacter* SpawnedAI = Cast<AAIBaseBattleCharacter>(SpawnedPawn))
            {
                SpawnedAI->SpawnerOwner = this;
                SpawnedAI->RandomItemEquip();
            }
            ++CurrentSpawnCount;
        }
    }
}