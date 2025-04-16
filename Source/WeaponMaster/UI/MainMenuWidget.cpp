// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenuWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Styling/SlateColor.h"
#include "Styling/SlateWidgetStyleAsset.h"
#include "SlateBasics.h"
#include "Base_Widgets/SingleSelectionWidget.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h" // PlayerController 포함

void UMainMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    MenuButtons.Empty();
    if (Button_Single)
    {
        MenuButtons.Add(Button_Single);
        Button_Single->OnClicked.AddDynamic(this, &UMainMenuWidget::OnSingleButtonClicked);
        Button_Single->OnHovered.AddDynamic(this, &UMainMenuWidget::OnButtonHovered);
        Button_Single->SetUserFocus(GetOwningPlayer());
    }
    if (Button_Multi)
    {
        MenuButtons.Add(Button_Multi);
        Button_Multi->OnClicked.AddDynamic(this, &UMainMenuWidget::OnMultiButtonClicked);
        Button_Multi->OnHovered.AddDynamic(this, &UMainMenuWidget::OnButtonHovered);
        SavedStyle = Button_Multi->WidgetStyle;
        Button_Multi->SetUserFocus(GetOwningPlayer());
    }
    if (Button_Shop)
    {
        MenuButtons.Add(Button_Shop);
        Button_Shop->OnClicked.AddDynamic(this, &UMainMenuWidget::OnShopButtonClicked);
        Button_Shop->OnHovered.AddDynamic(this, &UMainMenuWidget::OnButtonHovered);
        Button_Shop->SetUserFocus(GetOwningPlayer());
    }
    if (Button_Exit)
    {
       MenuButtons.Add(Button_Exit);
       Button_Exit->OnClicked.AddDynamic(this, &UMainMenuWidget::OnExitButtonClicked);
       Button_Exit->OnHovered.AddDynamic(this, &UMainMenuWidget::OnButtonHovered);
       Button_Exit->SetUserFocus(GetOwningPlayer());
    }
    CurrentButtonIndex = 0;
    UpdateButtonFocus();
    
    // UI 입력 모드 설정 (메인 메뉴 포커스)
    SetupUIInputMode(this);
}

// UI 입력 모드 설정 함수 추가
void UMainMenuWidget::SetupUIInputMode(UUserWidget* Widget)
{
    if (!Widget)
    {
        return;
    }
    
    APlayerController* PC = GetOwningPlayer();
    if (PC)
    {
        FInputModeUIOnly InputMode;
        InputMode.SetWidgetToFocus(Widget->TakeWidget());
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = true;
    }
}

//For Keyboard Event
FReply UMainMenuWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    FKey Key = InKeyEvent.GetKey();
    bool bIsHandled = false;
    if (Key == EKeys::Up)
    {
        OnDeleteButtonFocus();
        CurrentButtonIndex = (CurrentButtonIndex - 1 + MenuButtons.Num()) % MenuButtons.Num();
        UpdateButtonFocus();
        if (MoveSound)
        {
            PlaySound(MoveSound);
        }
        bIsHandled = true;
    }
    else if (Key == EKeys::Down)
    {
        OnDeleteButtonFocus();
        CurrentButtonIndex = (CurrentButtonIndex + 1) % MenuButtons.Num();
        UpdateButtonFocus();
        if (MoveSound)
        {
            PlaySound(MoveSound);
        }
        bIsHandled = true;
    }
    else if (Key == EKeys::Enter)
    {
        if (MenuButtons.IsValidIndex(CurrentButtonIndex))
        {
            UButton* ClickedButton = MenuButtons[CurrentButtonIndex];
            if (ClickedButton)
            {
                ClickedButton->OnClicked.Broadcast();
                if (SelectSound)
                {
                    PlaySound(SelectSound);
                }
                bIsHandled = true;
            }
        }
    }
    if (bIsHandled)
    {
        return FReply::Handled();
    }
    return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

//For Mouse
void UMainMenuWidget::OnButtonHovered()
{
    if (MenuButtons.IsValidIndex(CurrentButtonIndex))
    {
        UButton* FocusedButton = MenuButtons[CurrentButtonIndex];
        if (FocusedButton)
        {
            FString TestText = FString::Printf(TEXT("Index: %d"), CurrentButtonIndex);
            UpdateButtonStyle(FocusedButton, false);
        }
    }
}

void UMainMenuWidget::UpdateButtonFocus()
{
    if (MenuButtons.IsValidIndex(CurrentButtonIndex))
    {
        UButton* FocusedButton = MenuButtons[CurrentButtonIndex];
        if (FocusedButton)
        {
           // FocusedButton->SetKeyboardFocus();
            UpdateButtonStyle(FocusedButton, true);
        }
       
    }
}
void UMainMenuWidget::OnDeleteButtonFocus()
{
    if (MenuButtons.IsValidIndex(CurrentButtonIndex))
    {
        UButton* FocusedButton = MenuButtons[CurrentButtonIndex];
        if (FocusedButton)
        {
            UpdateButtonStyle(FocusedButton, false);
        }
    }
}
void UMainMenuWidget::UpdateButtonStyle(UButton* Button, bool bIsHovered)
{
    if (!Button)
    {
        return;
    }
    if (bIsHovered)
    {
        FButtonStyle TempStyle = SavedStyle; 
        TempStyle.Normal = SavedStyle.Hovered; // Hover 상태 Brush를 Normal에 대입하여 보여줌.
        Button->SetStyle(TempStyle);
        Button->SynchronizeProperties();
    }
    else
    {
        Button->SetStyle(SavedStyle);
        Button->SynchronizeProperties();
    }
}
void UMainMenuWidget::PlaySound(TObjectPtr<USoundBase> Sound)
{
    if (!ensure(Sound))
    {
        return;
    }
    UGameplayStatics::PlaySound2D(this, Sound);
}

// SingleSelectionWidget 보여주기 함수
void UMainMenuWidget::ShowSingleSelectionMenu()
{
    if (!ensure(SingleSelectionWidgetClass))
    {
        return;
    }
    
    // 기존 위젯이 있으면 제거
    if (SingleSelectionWidget)
    {
        SingleSelectionWidget->RemoveFromParent();
        SingleSelectionWidget = nullptr;
    }
    
    SingleSelectionWidget = CreateWidget<USingleSelectionWidget>(GetWorld(), SingleSelectionWidgetClass);
    if (SingleSelectionWidget)
    {
        // 위젯 속성 구성
        SingleSelectionWidget->bUseUITransition = true;
        SingleSelectionWidget->PrevWidgetClass = this->GetClass(); // 메인 메뉴 위젯 클래스
        SingleSelectionWidget->PreviousLevelName = MainMenuLevelName;
        SingleSelectionWidget->GameLevelName = GameLevelName;
        SingleSelectionWidget->CurrentScreenType = ESelectionScreenType::Character;
        
        // 메인 메뉴 위젯 숨기기
        this->RemoveFromParent();
        
        // 싱글 선택 위젯 표시
        SingleSelectionWidget->AddToViewport();
        
        // UI 입력 모드 설정
        SetupUIInputMode(SingleSelectionWidget);
    }
}

//////////////////////
/// 버튼이 클릭되면,
/// 각 기능 수행
//////////////////////
void UMainMenuWidget::OnSingleButtonClicked()
{
    if (SelectSound)
    {
        PlaySound(SelectSound);
    }
    
    // 싱글 선택 메뉴 직접 표시
    ShowSingleSelectionMenu();
    
    // HUD에 알림 (기존 코드와의 호환성 유지)
    OnSingleClicked.Broadcast();
}

void UMainMenuWidget::OnMultiButtonClicked()
{
    if (SelectSound)
    {
        PlaySound(SelectSound);
    }
    
    // 멀티플레이어 레벨로 직접 이동
    UGameplayStatics::OpenLevel(GetWorld(), MultiplayerLevelName);
    
    // HUD에 알림 (기존 코드와의 호환성 유지)
    OnMultiClicked.Broadcast();
}

void UMainMenuWidget::OnShopButtonClicked()
{
    if (SelectSound)
    {
        PlaySound(SelectSound);
    }
    
    // 상점 기능 (미구현)
    
    // HUD에 알림 (기존 코드와의 호환성 유지)
    OnShopClicked.Broadcast();
}

void UMainMenuWidget::OnExitButtonClicked()
{
    if (SelectSound)
    {
        PlaySound(SelectSound);
    }
    
    // 게임 종료 직접 처리
    APlayerController* PC = GetOwningPlayer();
    if (PC)
    {
        PC->ConsoleCommand("quit");
    }
    
    // HUD에 알림 (기존 코드와의 호환성 유지)
    OnExitClicked.Broadcast();
} 

void UMainMenuWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

    if (APlayerController* PC = GetOwningPlayer())
   {
       FInputModeUIOnly InputMode;
       InputMode.SetWidgetToFocus(this->TakeWidget());
       PC->SetInputMode(InputMode);
       //테스트용//
       PC->bShowMouseCursor = true;
   }
    CurrentButtonIndex = 0;
    UpdateButtonStyle(MenuButtons[CurrentButtonIndex], true);
}