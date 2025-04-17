// Fill out your copyright notice in the Description page of Project Settings.


#include "OptionWidget.h"
#include "Components/Slider.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UOptionWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (BackgroundVolumeSlider)
    {
        //BackgroundVolumeSlider->OnValueChanged.AddDynamic(this, &UOptionWidget::OnBackgroundVolumeChanged);

        BackgroundVolumeSlider->SetValue(0.5f);
    }

    if (VolumeSlider)
    {
       // VolumeSlider->OnValueChanged.AddDynamic(this, &UOptionWidget::OnVolumeChanged);

       VolumeSlider->SetValue(0.5f);
    }

    if (BackButton)
    {
        BackButton->OnClicked.AddDynamic(this, &UOptionWidget::OnBackClicked);
    }
}

//void UOptionWidget::OnBackgroundVolumeChanged(float Value)
//{
//    UE_LOG(LogTemp, Warning, TEXT("Background Volume: %f"), Value);
//}
//
//void UOptionWidget::OnVolumeChanged(float Value)
//{
//    UE_LOG(LogTemp, Warning, TEXT("SFX Volume: %f"), Value);
//}

void UOptionWidget::OnBackClicked()
{
	//OptionComplete.Broadcast();
    SetVisibility(ESlateVisibility::Hidden);
}
