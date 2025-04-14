#include "IndividualMatchStatusWidget.h"
#include "Components/ScrollBox.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "../CommonUI/PlayerStatusWidget.h"
#include "Components/ScrollBoxSlot.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void UIndividualMatchStatusWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    // 매치 타이틀 설정
    if (MatchTitleTextBlock)
    {
        MatchTitleTextBlock->SetText(FText::FromString(TEXT("개인전")));
    }
}

void UIndividualMatchStatusWidget::InitializePlayerStatus()
{
    if (!PlayerContainer || !PlayerStatusWidgetClass)
    {
        return;
    }

    ClearPlayerWidgets();
}

void UIndividualMatchStatusWidget::ClearPlayerWidgets()
{
    if (!PlayerContainer)
    {
        return;
    }

    PlayerContainer->ClearChildren();
    PlayerWidgets.Empty();
}

void UIndividualMatchStatusWidget::SetRemainTimer(const int32 TimeRemain) const
{
    if (!RemainTimerTextBlock) 
    {
        return;
    }
    
    // 시간을 분:초 형식으로 변환
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
    
void UIndividualMatchStatusWidget::InitializeDummyPlayerStatus(int32 TotalPlayers, int32 LocalPlayerID)
{
    if (!PlayerContainer || !PlayerStatusWidgetClass)
    {
        return;
    }

    ClearPlayerWidgets();
    MyPlayerID = LocalPlayerID;
    
    // 최대 표시 플레이어 수 제한
    if (TotalPlayers > MAX_VISIBLE_PLAYERS)
    {
        TotalPlayers = MAX_VISIBLE_PLAYERS;
    }

    // 각 플레이어에 대한 위젯 생성
    for (int32 i = 0; i < TotalPlayers; ++i)
    {
        UPlayerStatusWidget* NewPlayerWidget = CreateWidget<UPlayerStatusWidget>(GetWorld(), PlayerStatusWidgetClass);
        if (!NewPlayerWidget)
        {
            continue;
        }

        // 플레이어 ID 설정
        const int32 PlayerID = i;
        NewPlayerWidget->SetID(PlayerID);
        
        // 플레이어 이름 설정 (자신인 경우 표시)
        FString PlayerName = FString::Printf(TEXT("플레이어 %d"), i + 1);
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
        
        UE_LOG(LogTemp, Warning, TEXT("더미 플레이어 %d가 추가됨"), i + 1);
    }
    
    // 초기 정렬
    SortPlayersByScore();
}

void UIndividualMatchStatusWidget::UpdatePlayerDeaths(int32 PlayerID, int32 Deaths)
{
    // PlayerStatusWidget에 데스 정보 업데이트 기능이 있다고 가정
    if (UPlayerStatusWidget* PlayerWidget = PlayerWidgets.FindRef(PlayerID))
    {
        // 데스 정보 업데이트 함수가 PlayerStatusWidget에 추가되어야 함
        // PlayerWidget->UpdateDeaths(Deaths);
    }
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

void UIndividualMatchStatusWidget::SortPlayersByScore()
{
    if (!PlayerContainer)
    {
        return;
    }
    
    // 정렬을 위한 임시 배열
    TArray<UPlayerStatusWidget*> SortedWidgets;
    PlayerWidgets.GenerateValueArray(SortedWidgets);
    
    // 점수에 따라 정렬 (점수 필드가 PlayerStatusWidget에 추가되어야 함)
    // 현재는 ID 기준으로 정렬
    SortedWidgets.Sort([](const UPlayerStatusWidget& A, const UPlayerStatusWidget& B) {
        // 실제로는 점수로 정렬해야 함
        // return A.GetScore() > B.GetScore();
        return A.GetCharacterID() < B.GetCharacterID();
    });
    
    // 컨테이너를 비우고 정렬된 순서로 다시 추가
    PlayerContainer->ClearChildren();
    
    for (int32 i = 0; i < SortedWidgets.Num(); ++i)
    {
        UPlayerStatusWidget* Widget = SortedWidgets[i];
        if (!Widget)
        {
            continue;
        }
        
        if (UScrollBoxSlot* BoxSlot = Cast<UScrollBoxSlot>(PlayerContainer->AddChild(Widget)))
        {
            BoxSlot->SetPadding(FMargin(0, 0, 0, WidgetSpacing));
        }
    }
}