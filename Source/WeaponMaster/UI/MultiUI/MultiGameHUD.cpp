// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiGameHUD.h"
#include "WrapStatusWidget.h"
#include "GameFramework/PlayerState.h"

void AMultiGameHUD::BeginPlay()
{
	Super::BeginPlay();
   /* if (WrapStatusWidgetClass)
    {
        CreatePlayerWidgets();
    }*/
    LogMessage("BeginPlay HUD");
    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AMultiGameHUD::CreatePlayerWidgets, 5.0f, false);
	//CreatePlayerWidgets();
}

void AMultiGameHUD::CreatePlayerWidgets()
{
    if (!WrapStatusWidgetClass) return;

    UWrapStatusWidget* NewItem = CreateWidget<UWrapStatusWidget>(GetWorld(), WrapStatusWidgetClass);
    if (NewItem)
    {
        NewItem->InitializePlayerStatus();
        NewItem->AddToViewport();
    }
 
}

void AMultiGameHUD::LogMessage(const FString& Message)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, Message);
    }
}