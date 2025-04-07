// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStatusWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"


void UPlayerStatusWidget::NativeConstruct()
{
    Super::NativeConstruct();

    /**For Test*/
    if (PlayerNameText)
    {
        PlayerNameText->SetText(FText::FromString("Player"));
    }

    if (HealthBar)
    {
        HealthBar->SetPercent(1.0f);
    }
    //Test End
}

void UPlayerStatusWidget::UpdateHealth(float CurrentHealth, float MaxHealth)
{
    if (HealthBar)
    {
        HealthBar->SetPercent(CurrentHealth / MaxHealth);
    }
}

void UPlayerStatusWidget::SetPlayerName(const FString& Name)
{
    if (PlayerNameText)
    {
        PlayerNameText->SetText(FText::FromString(Name));
    }
}

void UPlayerStatusWidget::SetThumbnail(UTexture2D* Thumbnail)
{
    if (PlayerThumbnail && Thumbnail)
    {
        PlayerThumbnail->SetBrushFromTexture(Thumbnail);
    }
}