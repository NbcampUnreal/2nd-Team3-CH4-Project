// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenuWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Styling/SlateColor.h"
#include "Styling/SlateWidgetStyleAsset.h"
#include "SlateBasics.h"
#include "Engine/Engine.h"

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
}
//For Keyboard Event
FReply UMainMenuWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    LogMessage("NativeOnKeyDown");
    FKey Key = InKeyEvent.GetKey();
    bool bIsHandled = false;
    if (Key == EKeys::Up)
    {
        LogMessage("Up");
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
        LogMessage("Down");
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
        LogMessage("Enter");
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
            LogMessage(TestText);
            UpdateButtonStyle(FocusedButton, false);
        }
    }
}

void UMainMenuWidget::UpdateButtonFocus()
{
    LogMessage("UpdateButtonFocus");
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
    LogMessage("DeleteButtonFocus");
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
void UMainMenuWidget::LogMessage(const FString& Message)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, Message);
    }
}
void UMainMenuWidget::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
    Super::NativeOnFocusLost(InFocusEvent);

    //if (APlayerController* PC = GetOwningPlayer())
    //{
    //    FInputModeUIOnly InputMode;
    //    InputMode.SetWidgetToFocus(this->TakeWidget());
    //    PC->SetInputMode(InputMode);
    //    //테스트용
    //    PC->bShowMouseCursor = true;
    //}
    //LogMessage("Focus Reset");
}
//////////////////////
/// 버튼이 클릭되면,
/// MainHUD로 관리를 요청
//////////////////////
void UMainMenuWidget::OnSingleButtonClicked()
{
    LogMessage("SingleButtonClicked");
    OnSingleClicked.Broadcast();
}
void UMainMenuWidget::OnMultiButtonClicked()
{
    LogMessage("CoopButtonClicked");
    OnMultiClicked.Broadcast();
}
void UMainMenuWidget::OnShopButtonClicked()
{
    LogMessage("ShopButtonClicked");
    OnShopClicked.Broadcast();
}
void UMainMenuWidget::OnExitButtonClicked()
{
    LogMessage("ExitButtonClicked");
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
    LogMessage("Focus Reset");
}
