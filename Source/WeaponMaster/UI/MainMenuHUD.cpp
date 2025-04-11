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
        LogMessage("MainMenuWidgetClass가 설정되지 않았습니다!");
        return;
    }
    
    // 기존 위젯이 있으면 제거
    if (MainMenuWidget)
    {
        MainMenuWidget->RemoveFromParent();
        MainMenuWidget = nullptr;
    }
    
    MainMenuWidget = CreateWidget<UMainMenuWidget>(GetWorld(), MainMenuWidgetClass);
    if (MainMenuWidget)
    {
        MainMenuWidget->AddToViewport();
        
        // 델리게이트 연결 (MainMenuWidget에서 자체적으로 처리하지만 호환성을 위해 유지)
        MainMenuWidget->OnSingleClicked.AddDynamic(this, &AMainMenuHUD::HandleSingleClicked);
        MainMenuWidget->OnMultiClicked.AddDynamic(this, &AMainMenuHUD::HandleMultiClicked);
        MainMenuWidget->OnShopClicked.AddDynamic(this, &AMainMenuHUD::HandleShopClicked);
        MainMenuWidget->OnExitClicked.AddDynamic(this, &AMainMenuHUD::HandleExitClicked);
        
        LogMessage("메인 메뉴 위젯 표시됨");
    }
}

void AMainMenuHUD::HideMainMenu()
{
    if (MainMenuWidget)
    {
        // 델리게이트 연결 해제
        if (MainMenuWidget->OnSingleClicked.IsAlreadyBound(this, &AMainMenuHUD::HandleSingleClicked))
        {
            MainMenuWidget->OnSingleClicked.RemoveDynamic(this, &AMainMenuHUD::HandleSingleClicked);
        }
        if (MainMenuWidget->OnMultiClicked.IsAlreadyBound(this, &AMainMenuHUD::HandleMultiClicked))
        {
            MainMenuWidget->OnMultiClicked.RemoveDynamic(this, &AMainMenuHUD::HandleMultiClicked);
        }
        if (MainMenuWidget->OnShopClicked.IsAlreadyBound(this, &AMainMenuHUD::HandleShopClicked))
        {
            MainMenuWidget->OnShopClicked.RemoveDynamic(this, &AMainMenuHUD::HandleShopClicked);
        }
        if (MainMenuWidget->OnExitClicked.IsAlreadyBound(this, &AMainMenuHUD::HandleExitClicked))
        {
            MainMenuWidget->OnExitClicked.RemoveDynamic(this, &AMainMenuHUD::HandleExitClicked);
        }
        
        MainMenuWidget->RemoveFromParent();
        MainMenuWidget = nullptr;
        
        LogMessage("메인 메뉴 위젯 제거됨");
    }
}

// 이벤트 핸들러 (이제는 거의 사용되지 않지만 호환성을 위해 유지)
void AMainMenuHUD::HandleSingleClicked()
{
    LogMessage("MainMenuHUD::HandleSingleClicked - 이 함수는 더 이상 필요하지 않습니다");
}

void AMainMenuHUD::HandleMultiClicked()
{
    LogMessage("MainMenuHUD::HandleMultiClicked - 이 함수는 더 이상 필요하지 않습니다");
}

void AMainMenuHUD::HandleShopClicked()
{
    LogMessage("MainMenuHUD::HandleShopClicked - 이 함수는 더 이상 필요하지 않습니다");
}

void AMainMenuHUD::HandleExitClicked()
{
    LogMessage("MainMenuHUD::HandleExitClicked - 이 함수는 더 이상 필요하지 않습니다");
}