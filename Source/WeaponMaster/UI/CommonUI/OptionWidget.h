// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OptionWidget.generated.h"

class USlider;
class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOptionAction);


UCLASS()
class WEAPONMASTER_API UOptionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnOptionAction OptionComplete;
    virtual void NativeConstruct() override;
protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<USlider> BackgroundVolumeSlider;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<USlider> VolumeSlider;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> BackButton;
	
    UFUNCTION()
    void OnBackgroundVolumeChanged(float Value);

    UFUNCTION()
    void OnVolumeChanged(float Value);

    UFUNCTION()
    void OnBackClicked();

};
