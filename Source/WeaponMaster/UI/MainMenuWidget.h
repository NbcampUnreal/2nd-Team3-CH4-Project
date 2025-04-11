// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UButton;
class UTextBlock;
class USoundBase;
class USingleSelectionWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMainMenuAction);

UCLASS()
class WEAPONMASTER_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> Button_Single;
    
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> Button_Multi;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> Button_Shop;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> Button_Exit;

public:
    // 기존 델리게이트 (필요한 경우 HUD와의 통신용)
    UPROPERTY(BlueprintAssignable)
    FOnMainMenuAction OnSingleClicked;

    UPROPERTY(BlueprintAssignable)
    FOnMainMenuAction OnMultiClicked;

    UPROPERTY(BlueprintAssignable)
    FOnMainMenuAction OnShopClicked;
    
    UPROPERTY(BlueprintAssignable)
    FOnMainMenuAction OnExitClicked;

    // SingleSelectionWidget 직접 생성 및 관리를 위한 속성
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI|Widgets")
    TSubclassOf<USingleSelectionWidget> SingleSelectionWidgetClass;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI|Widgets")
    TObjectPtr<USingleSelectionWidget> SingleSelectionWidget;

    // 레벨 이름
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Levels")
    FName MultiplayerLevelName = TEXT("MainSessionMap");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Levels")
    FName MainMenuLevelName = TEXT("MainMenuMap");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Levels")
    FName GameLevelName = TEXT("Test_TravelBong");

protected:
    TArray<TObjectPtr<UButton>> MenuButtons;

    int32 CurrentButtonIndex;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
    TObjectPtr<USoundBase> MoveSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
    TObjectPtr<USoundBase> SelectSound;

    // 버튼 클릭 핸들러
    UFUNCTION()
    void OnSingleButtonClicked();

    UFUNCTION()
    void OnMultiButtonClicked();
  
    UFUNCTION()
    void OnShopButtonClicked();
    
    UFUNCTION()
    void OnExitButtonClicked();

    // SingleSelectionWidget 관련 함수
    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowSingleSelectionMenu();
    
    UFUNCTION()
    void HandleSingleSelectionPrevClicked();

    // UI 입력 모드 설정
    void SetupUIInputMode(UUserWidget* Widget);

    UFUNCTION()
    void OnButtonHovered();

    void UpdateButtonFocus();

    void PlaySound(TObjectPtr<USoundBase> Sound);

    void UpdateGoldText(int32 Gold);

    void LogMessage(const FString& Message);
    void UpdateButtonStyle(UButton* Button, bool bIsHovered);

    void OnDeleteButtonFocus();

    virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;

    FButtonStyle SavedStyle;

    virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
};