// Fill out your copyright notice in the Description page of Project Settings.


#include "WrapStatusWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "../CommonUI/PlayerStatusWidget.h"
#include "GameFramework/GameStateBase.h"
#include "Engine/World.h"

void UWrapStatusWidget::InitializePlayerStatus()
{
}

//void UWrapStatusWidget::InitializePlayerStatus()
//{
//	if (!PlayerContainer || !PlayerStatusWidgetClass) return;
//
//	PlayerContainer->ClearChildren();
//
//
//	AGameStateBase* GS = GetWorld()->GetGameState<AGameStateBase>();
//	if (!GS) return;
//	int16 PlayerNum = GS->PlayerArray.Num();
//	UE_LOG(LogTemp, Warning, TEXT("%d"),PlayerNum);
//	for (int i = 0; i < PlayerNum; i++)
//	{
//
//		UPlayerStatusWidget* NewItem = CreateWidget<UPlayerStatusWidget>(GetWorld(), PlayerStatusWidgetClass);
//		if (!NewItem)
//		{
//			UE_LOG(LogTemp, Error, TEXT("Failed to create PlayerStatusWidget for index %d"), i);
//			continue;
//		}
//
//		/*if (GS->PlayerArray[i])
//		{
//			FString PlayerName = GS->PlayerArray[i]->GetPlayerName();
//			if (PlayerName.IsEmpty())
//			{
//				PlayerName = FString::Printf(TEXT("Player %d"), i + 1);
//			}
//			NewItem->SetPlayerName(PlayerName);
//			NewItem->UpdateHealth(100.0f, 100.0f);
//		}*/
//
//		NewItem->SetVisibility(ESlateVisibility::Visible);
//
//		UHorizontalBoxSlot* BoxSlot = PlayerContainer->AddChildToHorizontalBox(NewItem);
//		if (BoxSlot)
//		{
//			BoxSlot->SetSize(ESlateSizeRule::Fill);
//			BoxSlot->SetPadding(FMargin(WidgetSpacing, 0, 0, 0));
//			BoxSlot->SetHorizontalAlignment(HAlign_Left);
//			//BoxSlot->SetVerticalAlignment(VAlign_Top);
//			UE_LOG(LogTemp, Warning, TEXT("Added player widget at index %d"), i);
//		}
//		else
//		{
//			UE_LOG(LogTemp, Error, TEXT("Failed to get HorizontalBoxSlot for index %d"), i);
//		}
//
//	}
//}
//
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
            NewItem->UpdateHealth(20.f * i,100.f);
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
            NewItem->UpdateHealth(10.f * i, 100.f);
            NewItem->SetID(i + 1 + Team1Target);
            if (UHorizontalBoxSlot* BoxSlot = RightTeamContainer->AddChildToHorizontalBox(NewItem))
            {
                ApplySlotSettings(BoxSlot);
                UE_LOG(LogTemp, Warning, TEXT("Dummy Player %d가 RightTeamContainer에 추가됨"), i + 1 + Team1Target);
            }
        }
    }
}
