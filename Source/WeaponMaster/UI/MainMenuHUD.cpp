// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuHUD.h"
#include "MainMenuWidget.h"
#include "ChoiceWidget.h"
#include "SelectWidget.h"
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
    if (MainMenuWidget)
    {
        MainMenuWidget->RemoveFromViewport();
        MainMenuWidget = nullptr;
    }
    //여기에서 다시 pc의 입력모드를 전환시켜야할까? 안해도 될것같은데 
}

void AMainMenuHUD::HandleSingleClicked()
{
    HideMainMenu();
    ShowChoiceMenu();
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

void AMainMenuHUD::ShowChoiceMenu()
{
    if (!ensure(ChoiceWidgetClass))
    {
        return;
    }
    if (ChoiceWidgetClass)
    {
        ChoiceWidget = CreateWidget<UChoiceWidget>(GetWorld(), ChoiceWidgetClass);
        if (ChoiceWidget)
        {
            ChoiceWidget->AddToViewport();
            ChoiceWidget->NextButtonClicked.AddDynamic(this, &AMainMenuHUD::HandleNextClicked);
            ChoiceWidget->PrevButtonClicked.AddDynamic(this, &AMainMenuHUD::HandlePrevClicked);
           /* if (APlayerController* PC = GetOwningPlayerController())
            {
                FInputModeUIOnly InputMode;
                InputMode.SetWidgetToFocus(ChoiceWidget->TakeWidget());
                PC->SetInputMode(InputMode);
                PC->bShowMouseCursor = true;
            }*/
        }
    }
}

void AMainMenuHUD::HideChoiceMenu()
{
    if (ChoiceWidget)
    {
        //가비지 컬렉터에 의해 사라지지만 혹시몰라서 강제로 들어가서 삭제 
        TArray<TObjectPtr<USelectWidget>> TempWidgets = ChoiceWidget->GetSelectWidgets();
        for (USelectWidget* Widget : TempWidgets)
        {
            Widget->RemoveFromViewport();
        }

        ChoiceWidget->RemoveFromViewport();
        ChoiceWidget = nullptr;
    }
}

void AMainMenuHUD::HandleNextClicked()
{
    //HideChoiceMenu();
    LogMessage("아직 미구현");
}

void AMainMenuHUD::HandlePrevClicked()
{
    HideChoiceMenu();
    ShowMainMenu();
}

