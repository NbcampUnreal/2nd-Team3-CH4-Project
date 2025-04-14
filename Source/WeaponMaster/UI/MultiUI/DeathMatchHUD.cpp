#include "DeathMatchHUD.h"
#include "IndividualMatchStatusWidget.h"
#include "KillLogWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"

ADeathMatchHUD::ADeathMatchHUD()
{
    // 기본값 설정
    bIsMenuVisible = false;
    LocalPlayerID = -1;
}

void ADeathMatchHUD::BeginPlay()
{
    Super::BeginPlay();
    
    // 플레이어 컨트롤러 가져오기
    APlayerController* PC = GetOwningPlayerController();
    if (PC)
    {
        // 로컬 플레이어 ID 가져오기 (PlayerState에서) 수정해야함
        if (PC->PlayerState)
        {
            LocalPlayerID = PC->PlayerState->GetPlayerId();
        }
    }
    
    InitializeHUD();
}

void ADeathMatchHUD::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

void ADeathMatchHUD::InitializeHUD()
{
    if (!IndividualMatchStatusWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("IndividualMatchStatusWidgetClass이 설정되지 않았습니다!"));
        return;
    }
    
    // 플레이어 컨트롤러 가져오기
    APlayerController* PC = GetOwningPlayerController();
    if (!PC)
    {
        return;
    }
    
    // 개인전 상태 위젯 생성
    IndividualMatchStatusWidget = CreateWidget<UIndividualMatchStatusWidget>(PC, IndividualMatchStatusWidgetClass);
    if (IndividualMatchStatusWidget)
    {
        IndividualMatchStatusWidget->AddToViewport();
        
        // 테스트용 더미 데이터 초기화 (실제 게임에서는 실제 플레이어 데이터로 대체)
        IndividualMatchStatusWidget->InitializeDummyPlayerStatus(6, LocalPlayerID);
    }
    
    // 인게임 메뉴 위젯 생성 (표시는 하지 않음)
    if (InGameMenuWidgetClass)
    {
        InGameMenuWidget = CreateWidget<UUserWidget>(PC, InGameMenuWidgetClass);
    }
}

void ADeathMatchHUD::UpdatePlayerStats(int32 PlayerID, int32 Kills, int32 Deaths)
{
    if (!IndividualMatchStatusWidget)
    {
        return;
    }
    
    // 플레이어 킬, 데스
    IndividualMatchStatusWidget->UpdatePlayerKills(PlayerID, Kills);
    IndividualMatchStatusWidget->UpdatePlayerDeaths(PlayerID, Deaths);
    
    // 연속적인 업데이트에 대한 정렬 요청을 줄이기 위해 타이머 사용
    if (!GetWorldTimerManager().IsTimerActive(SortPlayersTimerHandle))
    {
        GetWorldTimerManager().SetTimer(
            SortPlayersTimerHandle,
            [this]() { IndividualMatchStatusWidget->SortPlayersByScore(); },
            0.2f,  // 짧은 지연 시간
            false  // 반복 없음
        );
    }
}

void ADeathMatchHUD::UpdatePlayerHealth(int32 PlayerID, float CurrentHealth, float MaxHealth)
{
    if (!IndividualMatchStatusWidget)
    {
        return;
    }
    
    IndividualMatchStatusWidget->UpdatePlayerHealth(PlayerID, CurrentHealth, MaxHealth);
}

void ADeathMatchHUD::SetRemainingTime(int32 SecondsRemaining)
{
    if (!IndividualMatchStatusWidget)
    {
        return;
    }
    
    IndividualMatchStatusWidget->SetRemainTimer(SecondsRemaining);
}

void ADeathMatchHUD::ShowGameOverScreen(bool bIsWinner, int32 TotalScore)
{
    if (!GameOverWidgetClass)
    {
        return;
    }
    
    // 인게임 UI 숨기기
    if (IndividualMatchStatusWidget)
    {
        IndividualMatchStatusWidget->SetVisibility(ESlateVisibility::Hidden);
    }
    
    // 게임 종료 화면 표시
    APlayerController* PC = GetOwningPlayerController();
    if (!PC)
    {
        return;
    }
    
    GameOverWidget = CreateWidget<UUserWidget>(PC, GameOverWidgetClass);
    if (GameOverWidget)
    {
        GameOverWidget->AddToViewport();
        
        /*// 게임 결과 정보 설정
        UTextBlock* ResultTextBlock = Cast<UTextBlock>(GameOverWidget->GetWidgetFromName(TEXT("ResultText")));
        if (ResultTextBlock)
        {
            if (bIsWinner)
            {
                ResultTextBlock->SetText(NSLOCTEXT("DeathMatchHUD", "Victory", "승리!"));
            }
            else
            {
                ResultTextBlock->SetText(NSLOCTEXT("DeathMatchHUD", "Defeat", "패배!"));
            }
        }
        
        // 최종 점수 설정
        UTextBlock* ScoreTextBlock = Cast<UTextBlock>(GameOverWidget->GetWidgetFromName(TEXT("FinalScoreText")));
        if (ScoreTextBlock)
        {
            ScoreTextBlock->SetText(FText::Format(
                NSLOCTEXT("DeathMatchHUD", "FinalScore", "최종 점수: {0}"),
                FText::AsNumber(TotalScore)
            ));
        }*/
        
        // 마우스 커서 표시
        PC->SetShowMouseCursor(true);
        PC->SetInputMode(FInputModeUIOnly());
    }
}

void ADeathMatchHUD::ToggleInGameMenu()
{
    if (!InGameMenuWidget)
    {
        return;
    }
    
    APlayerController* PC = GetOwningPlayerController();
    if (!PC)
    {
        return;
    }
    
    bIsMenuVisible = !bIsMenuVisible;
    
    if (bIsMenuVisible)
    {
        // 메뉴 표시
        InGameMenuWidget->AddToViewport();
        PC->SetShowMouseCursor(true);
        PC->SetInputMode(FInputModeUIOnly());
        
        // 게임 일시 정지 (필요시)
        UGameplayStatics::SetGamePaused(GetWorld(), true);
    }
    else
    {
        // 메뉴 숨기기
        InGameMenuWidget->RemoveFromParent();
        PC->SetShowMouseCursor(false);
        PC->SetInputMode(FInputModeGameOnly());
        
        // 게임 재개 (필요시)
        UGameplayStatics::SetGamePaused(GetWorld(), false);
    }
}

void ADeathMatchHUD::SetHUDVisibility(bool bIsVisible)
{
    if (IndividualMatchStatusWidget)
    {
        IndividualMatchStatusWidget->SetVisibility(bIsVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
    }
}

bool ADeathMatchHUD::IsMenuVisible() const
{
    return bIsMenuVisible;
}

void ADeathMatchHUD::UpdateKillLog(const FText& Killer, const FText& Victim) const
{
    if (IndividualMatchStatusWidget)
    {
        IndividualMatchStatusWidget->KillLogContainer->AddKillLogEntry(Killer, Victim);
    }
}
