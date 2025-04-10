// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UButton;
class UTextBlock;
class USoundBase;

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
    UPROPERTY(BlueprintAssignable)
    FOnMainMenuAction OnSingleClicked;

    UPROPERTY(BlueprintAssignable)
    FOnMainMenuAction OnMultiClicked;

    UPROPERTY(BlueprintAssignable)
    FOnMainMenuAction OnShopClicked;
    
    UPROPERTY(BlueprintAssignable)
    FOnMainMenuAction OnExitClicked;

protected:
    TArray<TObjectPtr<UButton>> MenuButtons;

    int32 CurrentButtonIndex;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
    TObjectPtr<USoundBase> MoveSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
    TObjectPtr<USoundBase> SelectSound;

    UFUNCTION()
    void OnSingleButtonClicked();

    UFUNCTION()
    void OnMultiButtonClicked();
  
    UFUNCTION()
    void OnShopButtonClicked();
    
    UFUNCTION()
    void OnExitButtonClicked();

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
