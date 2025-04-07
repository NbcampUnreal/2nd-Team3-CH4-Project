// Fill out your copyright notice in the Description page of Project Settings.


#include "SingleGameHUD.h"
#include "../CommonUI/PlayerStatusWidget.h"

ASingleGameHUD::ASingleGameHUD()
{
}

void ASingleGameHUD::BeginPlay()
{
    Super::BeginPlay();

    if (PlayerStatusWidgetClass)
    {
        PlayerStatusWidget = CreateWidget<UPlayerStatusWidget>(GetOwningPlayerController(), PlayerStatusWidgetClass);
        if (PlayerStatusWidget)
        {
            PlayerStatusWidget->AddToViewport();
        }
    }
}
