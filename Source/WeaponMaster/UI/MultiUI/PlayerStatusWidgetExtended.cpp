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

void UPlayerStatusWidgetExtended::SetIsLocalPlayer(bool bIsLocal)
{
    bIsLocalPlayer = bIsLocal;
    
    // If this is the local player, visually indicate it
    if (bIsLocalPlayer)
    {
        // Find the border component if it exists in the widget hierarchy
        UBorder* BorderWidget = Cast<UBorder>(GetWidgetFromName(TEXT("PlayerBorder")));
        if (BorderWidget)
        {
            // Highlight the local player with a distinct color
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
}