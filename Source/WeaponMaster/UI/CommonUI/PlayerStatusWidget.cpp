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
    HideChatMessage();
    //Test End
}

void UPlayerStatusWidget::UpdateHealth(float CurrentHealth, float MaxHealth)
{
    if (HealthBar)
    {
        HealthBar->SetPercent(CurrentHealth / MaxHealth);
    }
}

void UPlayerStatusWidget::UpdateChat(const FString& NewMessage)
{
    if (!ChatText) return;
    if (NewMessage.IsEmpty())
    {
        ChatText->SetVisibility(ESlateVisibility::Hidden);
        ChatText->SetText(FText::GetEmpty());
        return;
    }

    ChatText->SetText(FText::FromString(NewMessage));
    ChatText->SetVisibility(ESlateVisibility::Visible);

    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(ChatHideTimerHandle);
        GetWorld()->GetTimerManager().SetTimer(ChatHideTimerHandle, this, &UPlayerStatusWidget::HideChatMessage, 5.f, false);
    }
}

void UPlayerStatusWidget::HideChatMessage()
{
    if (ChatText)
    {
        ChatText->SetVisibility(ESlateVisibility::Hidden);
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

