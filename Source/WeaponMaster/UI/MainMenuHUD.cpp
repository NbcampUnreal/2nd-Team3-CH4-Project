// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuHUD.h"
#include "MainMenuWidget.h"
#include "ChoiceWidget.h"
#include "SelectWidget.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

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
            MainMenuWidget->OnMultiClicked.AddDynamic(this, &AMainMenuHUD::HandleMultiClicked);
            MainMenuWidget->OnShopClicked.AddDynamic(this, &AMainMenuHUD::HandleShopClicked);
            MainMenuWidget->OnExitClicked.AddDynamic(this, &AMainMenuHUD::HandleExitClicked);
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
}

void AMainMenuHUD::HandleSingleClicked()
{
    HideMainMenu();
    ShowChoiceMenu();
}

void AMainMenuHUD::HandleMultiClicked()
{
    LogMessage("In Hud HandleMultiClicked");
    UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("MainSessionMap")));
}

void AMainMenuHUD::HandleShopClicked()
{
    LogMessage("In Hud HandleShopClicked");
}

void AMainMenuHUD::HandleExitClicked()
{
    APlayerController* PC = GetOwningPlayerController();
    if (PC)
    {
        PC->ConsoleCommand("quit");
    }
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
    PlaySound(SelectSound);
    UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("Test_TravelBong")));
}

void AMainMenuHUD::HandlePrevClicked()
{
    HideChoiceMenu();
    ShowMainMenu();
}

void AMainMenuHUD::PlaySound(const TObjectPtr<USoundBase>& Sound)
{
    if (!ensure(Sound))
    {
        return;
    }
    UGameplayStatics::PlaySound2D(this, Sound);
}
