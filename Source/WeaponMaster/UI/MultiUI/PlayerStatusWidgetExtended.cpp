#include "PlayerStatusWidgetExtended.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/Image.h"

void UPlayerStatusWidgetExtended::NativeConstruct()
{
    Super::NativeConstruct();
    
    // 초기값 설정
    Kills = 0;
    Deaths = 0;
    Score = 0;
    bIsLocalPlayer = false;
    
    // 초기 텍스트 업데이트
    UpdateTexts();
}

void UPlayerStatusWidgetExtended::UpdateKills(int32 NewKills)
{
    Kills = NewKills;
    UpdateTexts();
}

void UPlayerStatusWidgetExtended::UpdateDeaths(int32 NewDeaths)
{
    Deaths = NewDeaths;
    UpdateTexts();
}

void UPlayerStatusWidgetExtended::UpdateScore(int32 NewScore)
{
    Score = NewScore;
    UpdateTexts();
}

void UPlayerStatusWidgetExtended::SetIsLocalPlayer(bool bIsLocal)
{
    bIsLocalPlayer = bIsLocal;
    
    // 로컬 플레이어인 경우 시각적으로 강조
    if (bIsLocalPlayer)
    {
        // 부모 위젯에 Border 컴포넌트가 있다고 가정
        UBorder* BorderWidget = Cast<UBorder>(GetWidgetFromName(TEXT("PlayerBorder")));
        if (BorderWidget)
        {
            // 로컬 플레이어를 강조하기 위한 색상 설정
            BorderWidget->SetBrushColor(FLinearColor(0.1f, 0.5f, 1.0f, 0.3f));
        }
    }
}

void UPlayerStatusWidgetExtended::UpdateTexts()
{
    // 킬 텍스트 업데이트
    if (KillsText)
    {
        KillsText->SetText(FText::Format(
            NSLOCTEXT("PlayerStatusExtended", "Kills", "킬: {0}"),
            FText::AsNumber(Kills)
        ));
    }
    
    // 데스 텍스트 업데이트
    if (DeathsText)
    {
        DeathsText->SetText(FText::Format(
            NSLOCTEXT("PlayerStatusExtended", "Deaths", "데스: {0}"),
            FText::AsNumber(Deaths)
        ));
    }
    
    // 점수 텍스트 업데이트
    if (ScoreText)
    {
        ScoreText->SetText(FText::Format(
            NSLOCTEXT("PlayerStatusExtended", "Score", "점수: {0}"),
            FText::AsNumber(Score)
        ));
    }
}