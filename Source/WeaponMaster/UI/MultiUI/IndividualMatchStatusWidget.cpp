#include "IndividualMatchStatusWidget.h"

#include "KillLogWidget.h"
#include "PlayerStatusWidgetExtended.h"
#include "Components/ScrollBox.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "../CommonUI/PlayerStatusWidget.h"
#include "Components/ScrollBoxSlot.h"
#include "Engine/World.h"

void UIndividualMatchStatusWidget::NativeConstruct()
{
    Super::NativeConstruct();

    
}

void UIndividualMatchStatusWidget::InitializePlayerStatus()
{
    if (!PlayerContainer || !PlayerStatusWidgetClass) return;

    //ClearPlayerWidgets();
}

void UIndividualMatchStatusWidget::ClearPlayerWidgets()
{
    if (!PlayerContainer) return;

    PlayerContainer->ClearChildren();
    PlayerWidgets.Empty();
}

void UIndividualMatchStatusWidget::SetRemainTimer(const int32 TimeRemain) const
{
    if (!RemainTimerTextBlock) return;
    
    const int32 Minutes = TimeRemain / 60;
    const int32 Seconds = TimeRemain % 60;
    
    RemainTimerTextBlock->SetText(FText::Format(
        NSLOCTEXT("IndividualMatchWidget", "TimeRemaining", "{0}:{1}"),
        FText::AsNumber(Minutes),
        FText::AsNumber(Seconds).ToString().Len() == 1 
            ? FText::FromString(FString::Printf(TEXT("0%d"), Seconds)) 
            : FText::AsNumber(Seconds)
    ));
}

void UIndividualMatchStatusWidget::UpdateMatchTitle(const EMapType Map) const
{
    FText Title;
    
    switch (Map)
    {
    case EMapType::PVPMap:
        {
            Title = FText::FromString(TEXT("데스매치"));
            break;
        }
    case EMapType::PVEMap:
        {
            Title = FText::FromString(TEXT("협동모드"));
            break;
        }
    case EMapType::SessionMap:
        {
            Title = FText::GetEmpty();
            break;
        }
    }
    
    if (MatchTitleTextBlock)
    {
        MatchTitleTextBlock->SetText(Title);
    }
}

void UIndividualMatchStatusWidget::UpdatePlayer(int32 PID, FString& PlayerName)
{
    if (!PlayerContainer || !PlayerStatusWidgetClass) return;

    if (PlayerWidgets.Contains(PID))
    {
        UE_LOG(LogTemp, Warning, TEXT("Player ID %d already exists in UI, skipping."), PID);
        return;
    }
    
    UPlayerStatusWidgetExtended* NewPlayerWidget = CreateWidget<UPlayerStatusWidgetExtended>(GetWorld(), PlayerStatusWidgetClass);
    if (!NewPlayerWidget) return;

    // 플레이어 ID 설정
    const int32 PlayerID = PID;
    NewPlayerWidget->SetID(PlayerID);
        
    // 플레이어 이름 설정 (자신인 경우 표시)
    if (PlayerID == MyPlayerID)
    {
        PlayerName += TEXT(" (나)");
    }
    NewPlayerWidget->SetPlayerName(PlayerName);
    
    // 초기 체력 설정
    NewPlayerWidget->UpdateHealth(100.0f, 100.0f);
    
    // 가시성 설정
    NewPlayerWidget->SetVisibility(ESlateVisibility::Visible);
    
    // 스크롤 박스에 추가
    if (UScrollBoxSlot* BoxSlot = Cast<UScrollBoxSlot>(PlayerContainer->AddChild(NewPlayerWidget)))
    {
        BoxSlot->SetPadding(FMargin(0, 0, 0, WidgetSpacing));
    }
    
    // 맵에 저장
    PlayerWidgets.Add(PlayerID, NewPlayerWidget);
    
    ++PlayerIndex;
}

void UIndividualMatchStatusWidget::UpdatePlayerHealth(int32 PlayerID, float CurrentHealth, float MaxHealth)
{
    if (UPlayerStatusWidget* PlayerWidget = PlayerWidgets.FindRef(PlayerID))
    {
        PlayerWidget->UpdateHealth(CurrentHealth, MaxHealth);
    }
}

void UIndividualMatchStatusWidget::SetPlayerVisibility(int32 PlayerID, bool bIsVisible)
{
    if (UPlayerStatusWidget* PlayerWidget = PlayerWidgets.FindRef(PlayerID))
    {
        PlayerWidget->SetVisibility(bIsVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }
}

void UIndividualMatchStatusWidget::UpdatePlayerKills(int32 PlayerID, int32 Kills)
{
    if (UPlayerStatusWidgetExtended* PlayerWidget = PlayerWidgets.FindRef(PlayerID))
    {
        PlayerWidget->UpdateKills(Kills);
    }
}

void UIndividualMatchStatusWidget::UpdatePlayerName(int32 PlayerID, FString& PlayerName)
{
    if (UPlayerStatusWidgetExtended* PlayerWidget = PlayerWidgets.FindRef(PlayerID))
    {
        PlayerWidget->SetPlayerName(PlayerName);
    }
}

void UIndividualMatchStatusWidget::UpdatePlayerDeaths(const int32 PlayerID, const int32 Deaths) const
{
    if (UPlayerStatusWidgetExtended* PlayerWidget = PlayerWidgets.FindRef(PlayerID))
    {
        PlayerWidget->UpdateDeaths(Deaths);
    }
}