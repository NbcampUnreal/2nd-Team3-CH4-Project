// Fill out your copyright notice in the Description page of Project Settings.


#include "SingleGameHUD.h"
#include "../CommonUI/PlayerStatusWidget.h"
#include "../CommonUI/OptionMenuWidget.h"
ASingleGameHUD::ASingleGameHUD()
{

}

void ASingleGameHUD::BeginPlay()
{
    Super::BeginPlay();
    if (OptionMenuWidgetClass)
    {
        OptionMenuWidget = CreateWidget<UOptionMenuWidget>(GetOwningPlayerController(), OptionMenuWidgetClass);
        if (OptionMenuWidget)
        {
            OptionMenuWidget->SetVisibility(ESlateVisibility::Hidden);
            OptionMenuWidget->AddToViewport();
        }
    }
    if (APlayerController* PC = GetOwningPlayerController())
    {
        FInputModeGameAndUI InputMode;
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = true;
    }
    OpenSingleWidget();
}
void ASingleGameHUD::OpenSingleWidget()
{
    if (PlayerStatusWidgetClass)
    {
        PlayerStatusWidget = CreateWidget<UPlayerStatusWidget>(GetOwningPlayerController(), PlayerStatusWidgetClass);
        if (PlayerStatusWidget)
        {
            PlayerStatusWidget->SetPlayerName("Test");
            PlayerStatusWidget->UpdateHealth(100.f,100.f);
            //PlayerStatusWidget->SetThumbnail();
            PlayerStatusWidget->AddToViewport();
        }
    }
}
void ASingleGameHUD::SetPlayerHP(float NewHP)
{
	if (PlayerStatusWidget)
	{
		PlayerStatusWidget->UpdateHealth(NewHP, 100.f);
	}
}
void ASingleGameHUD::SetMenuWidget(bool bIsOpen)
{
    OptionMenuWidget->SetVisibility(bIsOpen ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}
