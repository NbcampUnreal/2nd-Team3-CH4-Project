// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuHUD.h"
#include "MainMenuWidget.h"
#include "Engine/Engine.h"

void AMainMenuHUD::BeginPlay()
{
    Super::BeginPlay();
    ShowMainMenu();
}

void AMainMenuHUD::LogMessage(const FString& Message)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, Message);
    }
}
void AMainMenuHUD::ShowMainMenu()
{
    if (!ensure(MainMenuWidgetClass))
    {
        return;
    }
    if (MainMenuWidgetClass)
    {
        MainMenuWidget = CreateWidget<UMainMenuWidget>(GetWorld(), MainMenuWidgetClass);
        if (MainMenuWidget)
        {
            MainMenuWidget->AddToViewport();
            MainMenuWidget->OnSingleClicked.AddDynamic(this, &AMainMenuHUD::HandleSingleClicked);
            MainMenuWidget->OnCoopClicked.AddDynamic(this, &AMainMenuHUD::HandleCoopClicked);
            MainMenuWidget->OnVsClicked.AddDynamic(this, &AMainMenuHUD::HandleVsClicked);
            MainMenuWidget->OnShopClicked.AddDynamic(this, &AMainMenuHUD::HandleShopClicked);
            MainMenuWidget->OnExitClicked.AddDynamic(this, &AMainMenuHUD::HandleExitClicked);

            if (APlayerController* PC = GetOwningPlayerController())
            {
                FInputModeUIOnly InputMode;
                InputMode.SetWidgetToFocus(MainMenuWidget->TakeWidget());
                PC->SetInputMode(InputMode);
                PC->bShowMouseCursor = true;
            }
        }
    }
}

void AMainMenuHUD::HideMainMenu()
{
}

void AMainMenuHUD::HandleSingleClicked()
{
    LogMessage("In Hud HandleSingleClicked");
}

void AMainMenuHUD::HandleCoopClicked()
{
    LogMessage("In Hud HandleCoopClicked");
}

void AMainMenuHUD::HandleVsClicked()
{
    LogMessage("In Hud HandleVsClicked");
}

void AMainMenuHUD::HandleShopClicked()
{
    LogMessage("In Hud HandleShopClicked");
}

void AMainMenuHUD::HandleExitClicked()
{
    LogMessage("In Hud HandleExitClicked");
}
