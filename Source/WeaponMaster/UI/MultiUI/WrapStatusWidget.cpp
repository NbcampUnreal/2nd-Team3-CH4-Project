// Fill out your copyright notice in the Description page of Project Settings.


#include "WrapStatusWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "../CommonUI/PlayerStatusWidget.h"
#include "GameFramework/GameStateBase.h"
#include "Engine/World.h"

void UWrapStatusWidget::InitializePlayerStatus()
{
	if (!PlayerContainer || !PlayerStatusWidgetClass) return;

	PlayerContainer->ClearChildren();


	AGameStateBase* GS = GetWorld()->GetGameState<AGameStateBase>();
	if (!GS) return;
	int16 PlayerNum = GS->PlayerArray.Num();
	UE_LOG(LogTemp, Warning, TEXT("%d"),PlayerNum);
	for (int i = 0; i < PlayerNum; i++)
	{

		UPlayerStatusWidget* NewItem = CreateWidget<UPlayerStatusWidget>(GetWorld(), PlayerStatusWidgetClass);
		if (!NewItem)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create PlayerStatusWidget for index %d"), i);
			continue;
		}

		/*if (GS->PlayerArray[i])
		{
			FString PlayerName = GS->PlayerArray[i]->GetPlayerName();
			if (PlayerName.IsEmpty())
			{
				PlayerName = FString::Printf(TEXT("Player %d"), i + 1);
			}
			NewItem->SetPlayerName(PlayerName);
			NewItem->UpdateHealth(100.0f, 100.0f);
		}*/

		NewItem->SetVisibility(ESlateVisibility::Visible);

		UHorizontalBoxSlot* BoxSlot = PlayerContainer->AddChildToHorizontalBox(NewItem);
		if (BoxSlot)
		{
			BoxSlot->SetSize(ESlateSizeRule::Fill);
			BoxSlot->SetPadding(FMargin(WidgetSpacing, 0, 0, 0));
			BoxSlot->SetHorizontalAlignment(HAlign_Left);
			//BoxSlot->SetVerticalAlignment(VAlign_Top);
			UE_LOG(LogTemp, Warning, TEXT("Added player widget at index %d"), i);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to get HorizontalBoxSlot for index %d"), i);
		}

	}
}