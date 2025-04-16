#include "DeathMatchHUD.h"
#include "IndividualMatchStatusWidget.h"
#include "KillLogWidget.h"
#include "Blueprint/UserWidget.h"
#include "Characters/WeaponMasterCharacter.h"
#include "Characters/BaseBattleCharacter/BaseBattleCharacter.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerController.h"
#include "PlayerState/WeaponMasterPlayerState.h"

ADeathMatchHUD::ADeathMatchHUD()
{
    // 기본값 설정
    bIsMenuVisible = false;
    LocalPlayerID = -1;
}

void ADeathMatchHUD::BeginPlay()
{
    Super::BeginPlay();
    
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

    FTimerHandle DelayHandle;
    GetWorld()->GetTimerManager().SetTimer(DelayHandle, this, &ADeathMatchHUD::TryBindPlayerState, 0.2f, false);
    
    // 개인전 상태 위젯 생성
    IndividualMatchStatusWidget = CreateWidget<UIndividualMatchStatusWidget>(PC, IndividualMatchStatusWidgetClass);
    if (IndividualMatchStatusWidget)
    {
        IndividualMatchStatusWidget->AddToViewport();
    }
    
    // 인게임 메뉴 위젯 생성 (표시는 하지 않음)
    if (InGameMenuWidgetClass)
    {
        InGameMenuWidget = CreateWidget<UUserWidget>(PC, InGameMenuWidgetClass);
    }
}

void ADeathMatchHUD::TryBindPlayerState()
{
    bool bAllBound = true;

    if (AGameStateBase* GS = GetWorld()->GetGameState())
    {
        for (APlayerState* PS : GS->PlayerArray)
        {
            if (AWeaponMasterPlayerState* WMPS = Cast<AWeaponMasterPlayerState>(PS))
            {
                if (!BoundPlayerStates.Contains(WMPS))
                {
                    BindPlayerStatusWidget(WMPS);
                    BoundPlayerStates.Add(WMPS);
                }
            }
            else
            {
                bAllBound = false;
            }
        }
    }

    if (!bAllBound)
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerState 아직 도착 안 한 플레이어 있음. 재시도 예정"));
        GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ADeathMatchHUD::TryBindPlayerState);
    }
}

void ADeathMatchHUD::BindPlayerStatusWidget(AWeaponMasterPlayerState* WMPS)
{
    if (!WMPS) return;
    
    UE_LOG(LogTemp, Warning, TEXT("KillCount Delegate 바인딩 완료: %s, Controller isLocal: %s"),
       *WMPS->GetPlayerName(),
       GetOwningPlayerController()->IsLocalController() ? TEXT("YES") : TEXT("NO"));
    
    // 변경 이벤트 바인딩
    WMPS->OnKillCountChanged.AddDynamic(this, &ADeathMatchHUD::UpdateKillCount);
    WMPS->OnDeathCountChanged.AddDynamic(this, &ADeathMatchHUD::UpdateDeathCount);
    WMPS->OnHealthChanged.AddDynamic(this, &ADeathMatchHUD::UpdateHealth);
}

void ADeathMatchHUD::UpdateHealth(AWeaponMasterPlayerState* OwnerPS, float CurrentHealth, float MaxHealth)
{
    UpdatePlayerHealth(OwnerPS->GetPlayerId(), CurrentHealth, MaxHealth);
}

void ADeathMatchHUD::UpdateKillCount(AWeaponMasterPlayerState* OwnerPS, int32 NewVal)
{
    if (!IndividualMatchStatusWidget) return;
    IndividualMatchStatusWidget->UpdatePlayerKills(OwnerPS->GetPlayerId(), NewVal);
}

void ADeathMatchHUD::UpdateDeathCount(AWeaponMasterPlayerState* OwnerPS, int32 NewVal)
{
    if (!IndividualMatchStatusWidget) return;
    IndividualMatchStatusWidget->UpdatePlayerDeaths(OwnerPS->GetPlayerId(), NewVal);
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
