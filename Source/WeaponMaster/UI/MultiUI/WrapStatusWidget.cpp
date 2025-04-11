#include "WrapStatusWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "../CommonUI/PlayerStatusWidget.h"
#include "Components/TextBlock.h"
#include "Engine/World.h"

void UWrapStatusWidget::InitializePlayerStatus()
{
    if (!LeftTeamContainer || !RightTeamContainer || !PlayerStatusWidgetClass)
    {
        return;
    }

    LeftTeamContainer->ClearChildren();
    RightTeamContainer->ClearChildren();
}

void UWrapStatusWidget::SetRemainTimer(const int32 TimeRemain) const
{
    if (!RemainTimerTextBlock) return;
    RemainTimerTextBlock->SetText( FText::Format(
        NSLOCTEXT("SessionWidget", "PlayerCount", "{0}"),
        FText::AsNumber(TimeRemain)
    ));
}
    
void UWrapStatusWidget::InitializeDummyPlayerStatus(int32 TotalPlayers, int32 MyTeamNum)
{
    if (!LeftTeamContainer || !RightTeamContainer || !PlayerStatusWidgetClass)
    {
        return;
    }

    LeftTeamContainer->ClearChildren();
    RightTeamContainer->ClearChildren();
    
    if (TotalPlayers > 6)
    {
        TotalPlayers = 6;
    }

    const int32 Team1Target = FMath::CeilToInt(TotalPlayers / 2.0f);
    const int32 Team2Target = TotalPlayers - Team1Target;

    auto ApplySlotSettings = [this](UHorizontalBoxSlot* BoxSlot)
        {
            if (BoxSlot)
            {
                BoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
                BoxSlot->SetHorizontalAlignment(HAlign_Fill);
                BoxSlot->SetVerticalAlignment(VAlign_Top);
                BoxSlot->SetPadding(FMargin(0, WidgetSpacing, 0, 0));
            }
        };

    if (MyTeamNum == 1)
    {
        for (int32 i = 0; i < Team1Target; ++i)
        {
            UPlayerStatusWidget* NewItem = CreateWidget<UPlayerStatusWidget>(GetWorld(), PlayerStatusWidgetClass);
            if (!NewItem)
            {
                continue;
            }

            NewItem->SetPlayerName(FString::Printf(TEXT("Dummy Player %d"), i + 1));
            NewItem->UpdateHealth(100.f,100.f);
            NewItem->SetID(i);
            NewItem->SetVisibility(ESlateVisibility::Visible);

            if (UHorizontalBoxSlot* BoxSlot = LeftTeamContainer->AddChildToHorizontalBox(NewItem))
            {
                ApplySlotSettings(BoxSlot);
                UE_LOG(LogTemp, Warning, TEXT("Dummy Player %d가 LeftTeamContainer에 추가됨"), i + 1);
            }
        }

        for (int32 i = 0; i < Team2Target; ++i)
        {
            UPlayerStatusWidget* NewItem = CreateWidget<UPlayerStatusWidget>(GetWorld(), PlayerStatusWidgetClass);
            if (!NewItem)
            {
                continue;
            }

            NewItem->SetPlayerName(FString::Printf(TEXT("Dummy Player %d"), i + 1 + Team1Target));
            NewItem->SetVisibility(ESlateVisibility::Visible);
            NewItem->UpdateHealth(100.f, 100.f);
            NewItem->SetID(i + 1 + Team1Target);
            if (UHorizontalBoxSlot* BoxSlot = RightTeamContainer->AddChildToHorizontalBox(NewItem))
            {
                ApplySlotSettings(BoxSlot);
                UE_LOG(LogTemp, Warning, TEXT("Dummy Player %d가 RightTeamContainer에 추가됨"), i + 1 + Team1Target);
            }
        }
    }
    
    if (MyTeamNum == 0)
    {
        
    }
}
