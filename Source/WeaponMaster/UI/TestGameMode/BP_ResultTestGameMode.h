// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BP_ResultTestGameMode.generated.h"

USTRUCT()
struct FPlayerStartUsage
{
    GENERATED_BODY()

    UPROPERTY()
    APlayerStart* PlayerStart;

    UPROPERTY()
    bool bIsUsed;
};

UCLASS()
class WEAPONMASTER_API ABP_ResultTestGameMode : public AGameMode
{
	GENERATED_BODY()
public:
    ABP_ResultTestGameMode();

    virtual void BeginPlay() override;
    virtual void PostLogin(APlayerController* NewPlayer) override;
    APlayerStart* FindAvailablePlayerStart(const FName& Tag);

protected:

   // UPROPERTY()
    TMap<FName, TArray<FPlayerStartUsage>> PlayerStartMap;

    UPROPERTY(EditDefaultsOnly, Category = "Camera")
    AActor* SpectatorCameraActor;

    UPROPERTY(EditDefaultsOnly, Category = "Pawn")
    TSubclassOf<APawn> ResultPawnClass;

    UPROPERTY()
    TMap<APlayerController*, FString> PlayerResultMap;

    UPROPERTY(EditAnywhere, Category = "Camera")
    TObjectPtr<AActor> PlayerWinCamera;

    UPROPERTY(EditAnywhere, Category = "Camera")
    TObjectPtr<AActor> PlayerLoseCamera;

    UPROPERTY(EditAnywhere, Category = "Camera")
    FString PlayerResult = "Winner";

    UPROPERTY(EditAnywhere, Category = "Camera")
    bool bIsWinner = true;

    bool bIsSaved = false;
    void SavedPlayerMap();

    //1번 그때그때 필요한 PlayerStart위치를 뽑아내고 
    //컨트롤러와 현재 내가 서있는 플레이어 스타트를 연결해놔야하고
    
    // BeginPlay에서 애니메이션을 재생하기 위해서는 
    //  아니다 이건 컨트롤러 그냥 순회하면 되겠다 
    // 
    // PlayerStart와 bool IsUsed를 엮어놔야돼

    //이렇게 알고리즘으 ㄹ짜면 
    // 1->위치 뽑고, 해당하는 PlayerStart 의 IsUsed를 변경해주고

    // 2->로그아웃 시 Controller의 PlayerStart뽑아내고 , IsUsed변경 
};
