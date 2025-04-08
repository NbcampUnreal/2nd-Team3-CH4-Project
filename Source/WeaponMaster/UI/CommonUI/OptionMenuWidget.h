// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OptionMenuWidget.generated.h"

class UButton;
class UOptionWidget;

UCLASS()
class WEAPONMASTER_API UOptionMenuWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

    UFUNCTION()
    void OnOptionCompleted();

protected:
    // 기존 버튼들
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> ResumeButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> MainMenuButton;


    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> OptionButton;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UOptionWidget> OptionWidgetClass;

    UPROPERTY()
    TObjectPtr<UOptionWidget> OptionWidget;

protected:
    UFUNCTION()
    void OnResumeClicked();

    UFUNCTION()
    void OnMainMenuClicked();

    UFUNCTION()
    void OnOptionClicked();

   
};
