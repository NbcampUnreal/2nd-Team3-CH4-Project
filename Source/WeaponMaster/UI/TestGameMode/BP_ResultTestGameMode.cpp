// Fill out your copyright notice in the Description page of Project Settings.


#include "BP_ResultTestGameMode.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraActor.h"
#include "BongResultCharacter.h"

/**PostLogin -> BeginPlay*/
//지금 해야할게 이렇게 함수를 짜지말고,
// 그때 그때 찾고 갱신해야하는데
ABP_ResultTestGameMode::ABP_ResultTestGameMode()
{
}

void ABP_ResultTestGameMode::BeginPlay()
{
    Super::BeginPlay();
    
}

APlayerStart* ABP_ResultTestGameMode::FindAvailablePlayerStart(const FName& Tag)
{
    if (!bIsSaved)
    {
        SavedPlayerMap();
    }
    
    if (PlayerStartMap.Contains(Tag))
    {
        UE_LOG(LogTemp, Display, TEXT("[FindAvailablePlayerStart] : Find Tag"));
        TArray<FPlayerStartUsage>& Array = PlayerStartMap[Tag];
        for (FPlayerStartUsage& Usage : Array)
        {
            if (!Usage.bIsUsed)
            {
                Usage.bIsUsed = true;  // 사용 처리
                return Usage.PlayerStart;
            }
            UE_LOG(LogTemp, Display, TEXT("[FindAvailablePlayerStart] : IsUsed"));
        }
    }
    UE_LOG(LogTemp, Display, TEXT("[FindAvailablePlayerStart] : No Find Tag"));

    //여기에서 추가해주고, usage에서  사용처리 해줘야지.

    return nullptr;
}

void ABP_ResultTestGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    UE_LOG(LogTemp, Display, TEXT("PostLogin 호출"));
    
    //플레이어 정보에서 결과물을 빼와야한다 (인스턴스)
    //플컨 Map에 저장

    FName TagToUse = bIsWinner ? FName("Winner") : FName("Loser");

    APlayerStart* SpawnStart = FindAvailablePlayerStart(TagToUse);
    if (SpawnStart == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("태그 %s에 해당하는 사용 가능한 PlayerStart가 없습니다."), *TagToUse.ToString());
        return;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    APawn* ResultPawn = GetWorld()->SpawnActor<APawn>(ResultPawnClass, SpawnStart->GetActorLocation(), SpawnStart->GetActorRotation(), SpawnParams);
    if (ResultPawn)
    {
        NewPlayer->Possess(ResultPawn);

        if (PlayerWinCamera && PlayerLoseCamera)
        {
            NewPlayer->SetViewTarget(bIsWinner ? PlayerWinCamera : PlayerLoseCamera);
            ABongResultCharacter* TempChar = Cast<ABongResultCharacter>(NewPlayer->GetPawn());
            
            if (TempChar)
            {
                /**정보 받아와서 바꿔야할 함수 위에서 bIsWinner만 지역변수로 저장해놓으면 될듯*/
                TempChar->Client_PlayRandomAnimation(bIsWinner);
            }

        }
    }
}
void ABP_ResultTestGameMode::SavedPlayerMap()
{
    
    UE_LOG(LogTemp, Display, TEXT("BeginPlay 호출"));
    for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
    {
        APlayerStart* Start = *It;
        if (Start)
        {
            if (Start->PlayerStartTag == FName("Winner"))
            {
                if (!PlayerStartMap.Contains(FName("Winner")))
                {
                    PlayerStartMap.Add(FName("Winner"), TArray<FPlayerStartUsage>());
                }
                FPlayerStartUsage NewUsage;
                NewUsage.PlayerStart = Start;
                NewUsage.bIsUsed = false;
                PlayerStartMap[FName("Winner")].Add(NewUsage);
                UE_LOG(LogTemp, Log, TEXT("PlayerSTART - Winner 추가됨"));
            }
            else if (Start->PlayerStartTag == FName("Loser"))
            {
                if (!PlayerStartMap.Contains(FName("Loser")))
                {
                    PlayerStartMap.Add(FName("Loser"), TArray<FPlayerStartUsage>());
                }
                FPlayerStartUsage NewUsage;
                NewUsage.PlayerStart = Start;
                NewUsage.bIsUsed = false;
                PlayerStartMap[FName("Loser")].Add(NewUsage);
                UE_LOG(LogTemp, Log, TEXT("PlayerSTART - Loser 추가됨"));
            }
        }
    }
    TArray<AActor*> FoundCameras;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACameraActor::StaticClass(), FoundCameras);
    for (AActor* Camera : FoundCameras)
    {
        if (Camera)
        {
            if (Camera->ActorHasTag(FName("Winner")))
            {
                PlayerWinCamera = Camera;
                UE_LOG(LogTemp, Log, TEXT("추가됨 - 승리 카메라: %s"), *Camera->GetName());
            }

        }
    }
    for (AActor* Camera : FoundCameras)
    {
        if (Camera)
        {
            if (Camera->ActorHasTag(FName("Loser")))
            {
                PlayerLoseCamera = Camera;
                UE_LOG(LogTemp, Log, TEXT("추가됨 - 패배 카메라: %s"), *Camera->GetName());
            }

        }
    }
    bIsSaved = true;
}